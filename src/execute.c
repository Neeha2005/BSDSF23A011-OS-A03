#include "../include/shell.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int execute_command(char **args) {
    if (!args[0]) return 1;

    // Check for built-ins first
    if (strcmp(args[0], "cd") == 0) {
        change_directory(args);
        return 1;
    }
    if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    }
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting MyShell...\n");
        exit(0);
    }
    if (strcmp(args[0], "history") == 0) {
        print_history();
        return 1;
    }

    // --- Handle pipes ---
    int pipe_index = -1;
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index != -1) {
        args[pipe_index] = NULL; // Split args for left command

        int fd[2];
        if (pipe(fd) == -1) {
            perror("myshell: pipe failed");
            return 1;
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            // Left child (writer)
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            if (execvp(args[0], args) == -1) perror("myshell");
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Right child (reader)
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            char **right_args = &args[pipe_index + 1];
            if (execvp(right_args[0], right_args) == -1) perror("myshell");
            exit(EXIT_FAILURE);
        }

        // Parent closes pipe ends
        close(fd[0]);
        close(fd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        return 1;
    }

    // --- Handle I/O redirection ---
    int in_fd = -1, out_fd = -1;
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            in_fd = open(args[i + 1], O_RDONLY);
            if (in_fd < 0) perror("myshell");
        }
        if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) perror("myshell");
        }
    }

    // --- Execute command ---
    pid_t pid = fork();
    if (pid == 0) {
        if (in_fd != -1) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != -1) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if (execvp(args[0], args) == -1) perror("myshell");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        waitpid(pid, NULL, 0);
    }

    return 1;
}
