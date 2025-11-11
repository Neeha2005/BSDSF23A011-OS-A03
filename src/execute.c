#include "../include/shell.h"
#include <errno.h>
#include <sys/wait.h>   // for waitpid()

/*
 * handle_builtin()
 * ----------------
 * Checks if the given command is one of the built-in commands.
 * Returns:
 *   1 → if the command was handled (built-in)
 *   0 → if it is not a built-in command
 */
int handle_builtin(char **args)
{
    if (args[0] == NULL)
        return 1;   // nothing to execute

    // exit: terminate the shell gracefully
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // cd: change current directory
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
        return 1;
    }

    // help: show list of built-ins
    else if (strcmp(args[0], "help") == 0) {
        printf("Simple Shell - Built-in commands:\n");
        printf("  cd <dir>   : Change directory\n");
        printf("  help       : Show this help message\n");
        printf("  exit       : Exit the shell\n");
        printf("  jobs       : Show background jobs (not implemented yet)\n");
        return 1;
    }

    // jobs: placeholder for future job control
    else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    // Not a built-in
    return 0;
}

/*
 * execute_command()
 * -----------------
 * Executes external commands using fork() and execvp().
 * The parent waits for the child to finish.
 */
void execute_command(char **args)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }
    else if (pid == 0) {
        // Child process: try to run external command
        if (execvp(args[0], args) == -1) {
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    }
    else {
        // Parent process: wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}
