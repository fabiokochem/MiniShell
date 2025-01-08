#include <stdio.h>
#include "jobs_command.c"  // Includes job management functionalities

// Function prototype for executing custom commands
int exec_custom_commands(char **args);

// Struct to define a custom command and its handler function
typedef struct {
    char* name;                      // Command name
    int (*function)(char **args);    // Pointer to the command handler function
} custom_command;

// Handler for the 'cd' (change directory) command
int cd_command(char **args) {
    if (args[1] == NULL) {
        printf("cd: expected argument to \"cd\"\n");
        return 1;
    } else {
        int err = chdir(args[1]);  // Change working directory
        return (err == -1) ? 1 : 0;
    }
}

// Handler for the 'pwd' (print working directory) command
int pwd_command(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);  // Print current working directory
        return 0;
    } else {
        return 1;
    }
}

// List of custom commands and their corresponding handler functions
custom_command custom_commands_list[] = {
    {"cd", cd_command},
    {"chdir", cd_command},
    {"pwd", pwd_command},
    {"jobs", list_jobs_command},  // Lists active jobs
    {"bg", handle_bg_command},    // Resumes a job in the background
    {"fg", handle_bg_command}     // Resumes a job in the foreground
};

// Executes a custom command if it matches one in the list
int exec_custom_commands(char **args) {
    int err = 0;

    // Iterate through the list of custom commands
    for (int i = 0; i < sizeof(custom_commands_list) / sizeof(custom_commands_list[0]); i++) {
        if (strcmp(args[0], custom_commands_list[i].name) == 0) {
            err = custom_commands_list[i].function(args);  // Call the command's handler function
            return err;
        }
    }

    // Return -1 if no custom command matches
    return -1;
}
