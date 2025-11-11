#include "../include/shell.h"

/*
 parse_pipeline:
   Input: a single segment string (no trailing ';' — caller splits by ';')
   Output: pointer to head of linked list of command_t representing pipeline stages.
   Behavior:
     - Splits by '|' into stages.
     - For each stage, tokenizes and fills args[], input_file, output_file, background flag.
     - Caller must not free tokens used as args because we strdup them here.
*/

static char *dup_token(const char *s) {
    return s ? strdup(s) : NULL;
}

command_t *parse_pipeline(char *segment) {
    if (!segment) return NULL;

    // We'll tokenize using strtok_r on '|' first to get stages
    char *saveptr_stage;
    char *stage = strtok_r(segment, "|", &saveptr_stage);
    command_t *head = NULL;
    command_t *prev = NULL;

    while (stage) {
        // Trim leading/trailing spaces of stage
        while (*stage && (*stage == ' ' || *stage == '\t')) stage++;
        char *end = stage + strlen(stage) - 1;
        while (end > stage && (*end == ' ' || *end == '\t' || *end == '\n')) { *end = '\0'; end--; }

        command_t *cmd = calloc(1, sizeof(command_t));
        cmd->input_file = NULL;
        cmd->output_file = NULL;
        cmd->background = 0;
        cmd->next = NULL;
        for (int i = 0; i < MAX_ARGS; ++i) cmd->args[i] = NULL;

        // Now parse tokens within the stage
        char *token;
        char *saveptr_tok;
        token = strtok_r(stage, " \t\r\n", &saveptr_tok);
        int ai = 0;
        while (token) {
            if (strcmp(token, "<") == 0) {
                token = strtok_r(NULL, " \t\r\n", &saveptr_tok);
                if (token) cmd->input_file = dup_token(token);
            } else if (strcmp(token, ">") == 0) {
                token = strtok_r(NULL, " \t\r\n", &saveptr_tok);
                if (token) cmd->output_file = dup_token(token);
            } else if (strcmp(token, "&") == 0) {
                // background marker for this pipeline (set on last command of pipeline)
                cmd->background = 1;
            } else {
                // normal arg
                if (ai < MAX_ARGS - 1) cmd->args[ai++] = dup_token(token);
            }
            token = strtok_r(NULL, " \t\r\n", &saveptr_tok);
        }
        cmd->args[ai] = NULL;

        // append to linked list
        if (!head) head = cmd;
        else prev->next = cmd;
        prev = cmd;

        // next stage
        stage = strtok_r(NULL, "|", &saveptr_stage);
    }

    // If last command has trailing '&' attached like "sleep 5 &", it was handled above because token '&' is separate.
    // If & was attached to previous token like "sleep 5&" (no space), it's uncommon; not handling that case.

    return head;
}

void free_pipeline(command_t *head) {
    command_t *cur = head;
    while (cur) {
        for (int i = 0; cur->args[i]; ++i) free(cur->args[i]);
        if (cur->input_file) free(cur->input_file);
        if (cur->output_file) free(cur->output_file);
        command_t *next = cur->next;
        free(cur);
        cur = next;
    }
}
