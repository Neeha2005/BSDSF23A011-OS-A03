#include "../include/shell.h"

/* Globals for background jobs (defined here) */
pid_t bg_jobs[MAX_BG_JOBS];
int bg_count = 0;

void change_directory(char **args) {
    if (!args[1]) {
        fprintf(stderr, "myshell: expected argument to 'cd'\n");
        return;
    }
    if (chdir(args[1]) != 0) {
        perror("myshell");
    }
}

void print_help(void) {
    printf("\n=== MyShell Built-in Commands ===\n");
    printf("cd <dir>     : Change directory\n");
    printf("help         : Show this help message\n");
    printf("history      : Show custom history\n");
    printf("jobs         : Show background jobs\n");
    printf("exit         : Exit the shell\n");
    printf("===============================\n\n");
}

void print_jobs(void) {
    printf("---- Background Jobs ----\n");
    if (bg_count == 0) {
        printf("No background jobs\n");
        return;
    }
    for (int i = 0; i < bg_count; ++i) {
        printf("%d: PID %d\n", i + 1, bg_jobs[i]);
    }
}
