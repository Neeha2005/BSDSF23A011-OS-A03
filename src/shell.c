#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

char *command_generator(const char *text, int state) {
    static int list_index, len;
    static char *commands[] = {"cd", "help", "exit", "jobs", "history", NULL};

    if (!state) { list_index = 0; len = strlen(text); }

    while (commands[list_index]) {
        char *name = commands[list_index++];
        if (strncmp(name, text, len) == 0) return strdup(name);
    }

    DIR *dir = opendir(".");
    struct dirent *entry;
    while (dir && (entry = readdir(dir))) {
        if (strncmp(entry->d_name, text, len) == 0) {
            closedir(dir);
            return strdup(entry->d_name);
        }
    }
    if (dir) closedir(dir);
    return NULL;
}

char **myshell_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 0;
    return rl_completion_matches(text, command_generator);
}

void shell_loop() {
    char *input_line;
    char **args;
    int running = 1;

    rl_attempted_completion_function = myshell_completion;
    printf("Welcome to MyShell (Features 2,3,4)\n");

    while (running) {
        input_line = readline("myshell> ");
        if (!input_line) { printf("\n"); break; }
        if (strlen(input_line) == 0) { free(input_line); continue; }

        if (input_line[0] == '!') {
            int n = atoi(&input_line[1]);
            char *hist_cmd = get_history_command(n);
            if (hist_cmd) {
                free(input_line);
                input_line = strdup(hist_cmd);
                printf("%s\n", input_line);
            } else {
                printf("No such command in history.\n");
                free(input_line);
                continue;
            }
        }

        add_to_history(input_line);
        add_history(input_line);

        char *token, *saveptr;
        int bufsize = 64, position = 0;
        args = malloc(bufsize * sizeof(char *));
        token = strtok_r(input_line, " \t\r\n", &saveptr);
        while (token != NULL) { args[position++] = token; token = strtok_r(NULL, " \t\r\n", &saveptr); }
        args[position] = NULL;

        if (!handle_builtin(args))
            execute_command(args);

        free(args);
        free(input_line);
    }
}
