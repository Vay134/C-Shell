#include "shell.h"

const char *builtin_commands[] = {
    "cd",      // Changes the current directory of the shell to the specified path. If no path is given, it defaults to the user's home directory.
    "help",    // List all builtin commands in the shell
    "exit",    // Exits the shell
    "usage",   // Provides a brief usage guide for the shell and its built-in command
    "env",     // Lists all the environment variables currently set in the shell
    "setenv",  // Sets or modifies an environment variable for this shell session
    "unsetenv" // Removes an environment variable from the shell
};

// Function that returns the number of builtin commands
int num_builtin_functions() {
    return sizeof(builtin_commands) / sizeof(char *);
}

/*** This is array of functions, with argument char ***/
int (*builtin_command_func[])(char **) = {
    &shell_cd,     // builtin_command_func[0]: cd 
    &shell_help,   // builtin_command_func[1]: help
    &shell_exit,   // builtin_command_func[2]: exit
    &shell_usage,  // builtin_command_func[3]: usage
    &list_env,     // builtin_command_func[4]: env
    &set_env_var,  // builtin_command_func[5]: setenv
    &unset_env_var // builtin_command_func[6]: unsetenv
};

// Function to change working directory
int shell_cd(char **args) {
    char cwd[1024];
    char target_path[PATH_MAX];
    int status;
    if (args[1] == NULL) {
        return 1;
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        snprintf(target_path, sizeof(target_path), "%s/%s", cwd, args[1]);
        printf("%s\n",target_path);
        status = chdir(target_path);
        if (status == 0) {
            return 1;
        }
        else {
            printf("Failed to change directory.\n");
            return -1;
        }
    }
    else
    {
        printf("Failed to get current working directory.\n");
        return -1;
    }
}

// Function to print out all builtin commands in the shell
int shell_help(char **args) {
    printf("The following builtin commands are implemented: \n");
    for (int i = 0; i < num_builtin_functions(); i++){
        printf("\t%s\n", builtin_commands[i]);
    }
    return 1;
}

int shell_exit(char **args) {
    return 0;
}

int shell_usage(char **args) {
    return -1;
}
int list_env(char **args) {
    return -1;
}
int set_env_var(char **args) {
    return -1;
}
int unset_env_var(char **args) {
    return -1;
}