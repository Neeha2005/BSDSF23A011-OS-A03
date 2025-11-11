#include "../include/shell.h"

#define MAX_LINE 1024
#define MAX_ARGS 64

void shell_loop(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char *token;

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;

        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        // Tokenize input
        int i = 0;
        token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Skip empty input
        if (args[0] == NULL)
            continue;

        // Handle built-in or external commands
        // Handle built-in or external commands
	if (handle_builtin(args))
    	    continue;  // already handled internally
	else
	    execute_command(args);  // not a built-in, run normally

    }
}
