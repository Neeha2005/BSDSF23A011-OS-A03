#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_TOKENS 64
#define MAX_INPUT_SIZE 1024
#define HISTORY_SIZE 20

// Shell functions
void shell_loop();
char **tokenize_input(char *input);
int execute_command(char **args);
void add_to_history(const char *command);
void print_history();
char* get_history_command(int n);
void change_directory(char **args);
void print_help();
int handle_builtin(char **args);

#endif
