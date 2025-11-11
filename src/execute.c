#include "../include/shell.h"
#include <errno.h>

extern pid_t bg_jobs[MAX_BG_JOBS];
extern int bg_count;

/* Helper to run a pipeline of command_t nodes (head). Supports multiple stages.
   Returns 1 on success, 0 on failure (doesn't abort shell).
*/
int execute_pipeline(command_t *head) {
    if (!head || !head->args[0]) return 1;

    // If pipeline has single stage and is a builtin, handle here
    if (!head->next) {
        char *cmd0 = head->args[0];
        if (strcmp(cmd0, "cd") == 0) { change_directory(head->args); return 1; }
        if (strcmp(cmd0, "help") == 0) { print_help(); return 1; }
        if (strcmp(cmd0, "history") == 0) { show_history(); return 1; }
        if (strcmp(cmd0, "jobs") == 0) { print_jobs(); return 1; }
        if (strcmp(cmd0, "exit") == 0) { exit(0); }
    }

    // Count stages
    int stages = 0;
    for (command_t *c = head; c; c = c->next) stages++;

    // Create pipes: for N stages we need N-1 pipes
    int (*pipes)[2] = NULL;
    if (stages > 1) {
        pipes = malloc(sizeof(int[2]) * (stages - 1));
        for (int i = 0; i < stages - 1; ++i) {
            if (pipe(pipes[i]) < 0) { perror("pipe"); free(pipes); return 0; }
        }
    }

    pid_t *pids = calloc(stages, sizeof(pid_t));
    int idx = 0;
    command_t *cur = head;
    while (cur) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            // cleanup: close pipes
            for (int i = 0; i < stages - 1; ++i) { close(pipes[i][0]); close(pipes[i][1]); }
            free(pipes); free(pids);
            return 0;
        } else if (pid == 0) {
            // Child: set up stdin/stdout redirection & pipes
            // If first stage and has input_file
            if (cur->input_file) {
                int fd = open(cur->input_file, O_RDONLY);
                if (fd < 0) { perror("open input"); exit(1); }
                dup2(fd, STDIN_FILENO); close(fd);
            }
            // If last stage and has output_file
            if (!cur->next && cur->output_file) {
                int fd = open(cur->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { perror("open output"); exit(1); }
                dup2(fd, STDOUT_FILENO); close(fd);
            }

            // If pipeline, connect pipes
            if (stages > 1) {
                if (idx == 0) {
                    // first stage: stdout -> pipes[0][1]
                    dup2(pipes[0][1], STDOUT_FILENO);
                } else if (!cur->next) {
                    // last stage: stdin <- pipes[idx-1][0]
                    dup2(pipes[idx - 1][0], STDIN_FILENO);
                } else {
                    // middle stage: stdin <- pipes[idx-1][0], stdout -> pipes[idx][1]
                    dup2(pipes[idx - 1][0], STDIN_FILENO);
                    dup2(pipes[idx][1], STDOUT_FILENO);
                }
                // Close all pipe fds in child
                for (int j = 0; j < stages - 1; ++j) {
                    close(pipes[j][0]); close(pipes[j][1]);
                }
            }

            // Exec
            execvp(cur->args[0], cur->args);
            // If exec fails:
            perror("myshell");
            exit(127);
        } else {
            // parent
            pids[idx++] = pid;
            cur = cur->next;
        }
    }

    // Parent: close all pipe ends
    if (stages > 1) {
        for (int i = 0; i < stages - 1; ++i) {
            close(pipes[i][0]); close(pipes[i][1]);
        }
    }

    // If background flag set on head (we treat it as pipeline-level background),
    // don't wait and add all child pids to bg_jobs
    int background = head->background;
    if (background) {
        for (int i = 0; i < stages; ++i) {
            if (bg_count < MAX_BG_JOBS) bg_jobs[bg_count++] = pids[i];
        }
        printf("[BG] PIDs:");
        for (int i = 0; i < stages; ++i) printf(" %d", pids[i]);
        printf("\n");
        free(pipes); free(pids);
        return 1;
    }

    // Otherwise wait for all children
    for (int i = 0; i < stages; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    free(pipes);
    free(pids);
    return 1;
}
