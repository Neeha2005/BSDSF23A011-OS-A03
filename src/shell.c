#include "../include/shell.h"

/* Tab completion: complete builtin commands or filenames */
static char *commands_list[] = {"cd", "help", "exit", "history", "jobs", NULL};

static char *command_generator(const char *text, int state) {
    static int idx, len;
    if (!state) { idx = 0; len = strlen(text); }
    while (commands_list[idx]) {
        char *c = commands_list[idx++];
        if (strncmp(c, text, len) == 0) return strdup(c);
    }
    // file completion fallback
    DIR *d = opendir(".");
    if (!d) return NULL;
    struct dirent *ent;
    while ((ent = readdir(d))) {
        if (strncmp(ent->d_name, text, len) == 0) {
            char *r = strdup(ent->d_name);
            closedir(d);
            return r;
        }
    }
    closedir(d);
    return NULL;
}

static char **myshell_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 0;
    return rl_completion_matches(text, command_generator);
}

void shell_loop(void) {
    rl_attempted_completion_function = myshell_completion;
    printf("Welcome to MyShell (v6 - features 1..6)\n");

    while (1) {
        char *line = readline("myshell> ");
        if (!line) { printf("\n"); break; }
        if (strlen(line) == 0) { free(line); continue; }

        // support !n to re-execute from custom history: naive implementation
        if (line[0] == '!') {
            int n = atoi(line + 1);
            // We didn't implement retrieval by index in custom history for brevity here.
            // You may extend add_to_history_custom to store and retrieve.
            printf("!n not implemented in this build (use history to see commands)\n");
            free(line);
            continue;
        }

        add_history(line);               // readline history
        add_to_history_custom(line);     // custom history

        // Split input by ';' into segments
        char *saveptr;
        char *segment = strtok_r(line, ";", &saveptr);
        while (segment) {
            // Trim leading/trailing
            while (*segment == ' ' || *segment == '\t') segment++;
            char *end = segment + strlen(segment) - 1;
            while (end > segment && (*end == ' ' || *end == '\t' || *end == '\n')) { *end = '\0'; end--; }

            // parse pipeline from segment
            char *segment_copy = strdup(segment); // parse_pipeline modifies string via strtok
            command_t *head = parse_pipeline(segment_copy);
            if (head) {
                // If last command in pipeline has '&' at end of args, set background for whole pipeline
                // Some users type '&' separated by space; parser sets background flag per command.
                // We'll treat pipeline background if last stage has background flag.
                // Check the last stage:
                command_t *last = head;
                while (last->next) last = last->next;
                if (last->background) head->background = 1;

                execute_pipeline(head);
                free_pipeline(head);
            }
            free(segment_copy);
            segment = strtok_r(NULL, ";", &saveptr);
        }

        free(line);

        // reap finished background jobs (non-blocking)
        int status;
        pid_t pid;
        for (int i = 0; i < bg_count; ) {
            pid = waitpid(bg_jobs[i], &status, WNOHANG);
            if (pid == 0) {
                ++i; // still running
            } else if (pid == -1) {
                // error or already reaped
                // remove from list
                for (int j = i; j < bg_count - 1; ++j) bg_jobs[j] = bg_jobs[j+1];
                --bg_count;
            } else {
                // finished
                printf("[BG DONE] PID %d\n", bg_jobs[i]);
                for (int j = i; j < bg_count - 1; ++j) bg_jobs[j] = bg_jobs[j+1];
                --bg_count;
            }
        }
    }
}
