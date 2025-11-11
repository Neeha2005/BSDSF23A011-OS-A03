#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_TOKENS 128
#define MAX_INPUT_SIZE 2048
#define HISTORY_SIZE 200
#define MAX_BG_JOBS 128
#define MAX_ARGS 64

typedef struct command {
    char *args[MAX_ARGS];    // NULL-terminated argv for this command
    char *input_file;        // filename for '<' or NULL
    char *output_file;       // filename for '>' or NULL
    int background;          // 1 if command should run in background (&)
    struct command *next;    // next command in pipeline (for '|')
} command_t;

/* Globals: defined once in shell.c and referenced via extern */
extern pid_t bg_jobs[MAX_BG_JOBS];
extern int bg_count;

/* shell loop */
void shell_loop(void);

/* parser & tokenizer */
command_t *parse_pipeline(char *segment);   // parse one segment (possible pipeline)
void free_pipeline(command_t *head);

/* executor */
int execute_pipeline(command_t *head);

/* built-ins & helpers */
void change_directory(char **args);
void print_help(void);
void print_jobs(void);

/* history */
void add_to_history_custom(const char *cmd);
void show_history(void);

#endif // SHELL_H
