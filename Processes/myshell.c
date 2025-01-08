#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include "custom_commands.c"  // Includes custom command implementations

// Function prototypes
int execute(char **args, bool background, bool doPipe);
int multipleCommands(char **args, int n);
void handler_signal (int sig);
int split (char *buffer, char **word);

// Global variables
int lastPipe = -1;              // Keeps track of the last pipe's read-end descriptor
pid_t foreground_pid = -1;      // Tracks the foreground process ID

int main(int argc, char *argv[]) {
    char buffer[1024];          // Input buffer
    char *word[1024];           // Tokenized command arguments
    int nb;

    // Setup signal handlers for SIGINT and SIGTSTP
    signal(SIGINT, handler_signal);
    signal(SIGTSTP, handler_signal);

    while (1) {
        // Main shell loop
        printf("Running myshell\n");
        printf("> ");
        fgets(buffer, 1024, stdin);       // Read user input
        int n = split(buffer, word);     // Split input into tokens
        printf("----------------------------------\n");
        multipleCommands(word, n);      // Handle multiple commands
        printf("----------------------------------\n");
    }
}

// Processes multiple commands separated by connectors (e.g., &&, ||, |, &)
int multipleCommands(char **args, int n) {
    char **start = args;
    int type = 0, err = 0, execNext = 1;
    for (int i = 0; i < n; i++) {
        // Identify command connector type
        if (args[i] == NULL) type = 3;
        else if (strcmp(args[i], "&&") == 0) type = 1;
        else if (strcmp(args[i], "||") == 0) type = 2;
        else if (strcmp(args[i], "&") == 0) type = 4;
        else if (strcmp(args[i], "|") == 0) type = 5;
        else if (strcmp(args[i], ";") == 0) type = 6;

        if (type > 0) {
            args[i] = NULL;  // Split commands at connector
            if (execNext) err = execute(start, type == 4, type == 5);
            execNext = ((type == 1 && err == 0) || (type == 2 && err != 0) || type > 2);
            start = &args[i + 1];
        }
        type = 0;
    }
    // Close the last pipe if used
    if (lastPipe != -1) close(lastPipe);
}

// Handles I/O redirection for commands
void redirect(char **args){
    int i;
    for (i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, 1);  // Redirect stdout
            close(fd);
        } else if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_RDONLY);
            dup2(fd, 0);  // Redirect stdin
            close(fd);
        } else if (strcmp(args[i], ">>") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_APPEND | O_WRONLY | O_CREAT, 0644);
            dup2(fd, 1);  // Append to stdout
            close(fd);
        } else if (strcmp(args[i], "2>") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, 2);  // Redirect stderr
            close(fd);
        } else if (strcmp(args[i], "2>>") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_APPEND | O_WRONLY | O_CREAT, 0644);
            dup2(fd, 2);  // Append to stderr
            close(fd);
        }
    }
}

// Signal handler for SIGINT and SIGTSTP
void handler_signal (int sig) {
    if (sig == SIGINT) {
        if (foreground_pid != -1) {
            kill(foreground_pid, SIGINT);  // Forward SIGINT to foreground process
        } else {
            exit(0);  // Exit shell
        }
    } else if (sig == SIGTSTP) {
        if (foreground_pid != -1) {
            kill(foreground_pid, SIGTSTP);  // Suspend foreground process
            foreground_pid = -1;
        }
    }
}

// Executes a command with options for background and piping
int execute(char **args, bool background, bool doPipe) {
    int fd[2];
    pid_t pid = -1;

    if (doPipe) {
        int err = pipe(fd);
        if (err == -1) {
            perror("pipe");
            return -1;
        }
    }

    pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork");
        return -1;
    }

    switch (pid) {
    case 0:
        // Child process
        if (lastPipe != -1) {
            dup2(lastPipe, 0);  // Connect input to last pipe
            close(lastPipe);
            lastPipe = -1;
        }

        if (doPipe) {
            close(fd[0]);  // Close unused read end of pipe
            dup2(fd[1], 1);  // Redirect stdout to pipe
            close(fd[1]);
        }

        redirect(args);  // Handle I/O redirection
        int err = exec_custom_commands(args);  // Check custom commands
        if (err != -1) return err;
        else err = execv(args[0], args);  // Execute command
        if (err == -1) err = execvp(args[0], args);  // Search PATH if needed
        perror("exec");
        exit(errno);  // Exit child process on error
    default:
        // Parent process
        if (lastPipe != -1) {
            close(lastPipe);  // Close previous pipe's read end
            lastPipe = -1;
        }

        if (doPipe) {
            close(fd[1]);  // Close write end of pipe
            lastPipe = fd[0];  // Save read end of pipe
        }

        if (!background) {
            int status;
            foreground_pid = pid;  // Set foreground process ID
            waitpid(pid, &status, WUNTRACED);  // Wait for child process
            foreground_pid = -1;
            return WEXITSTATUS(status);  // Return exit status of child
        } else {
            add_job(pid, "Running", args[0], background);  // Add to background jobs
            return 0;
        }
    }
}

// Splits input string into tokens
int split (char *buffer, char **word) {
    char *t = buffer;
    int i = 0;
    while ((word[i++] = strtok(t, " \f\n\r\t\v")) != NULL) {
        t = NULL;
    }
    return i;
}
