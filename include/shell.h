#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function declarations
void shell_loop(void);
void execute_command(char **args);
int handle_builtin(char **args);

#endif
