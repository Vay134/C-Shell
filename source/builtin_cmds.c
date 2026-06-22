#include "shell.h"

const char *builtin_commands[] = {
    "cd",      // Changes the current directory of the shell to the specified path. If no path is given, it defaults to the user's home directory.
    "help",    // List all builtin commands in the shell
    "exit",    // Exits the shell
    "usage",   // Provides a brief usage guide for the shell and its built-in command
    "env",     // Lists all the environment variables currently set in the shell
    "setenv",  // Sets or modifies an environment variable for this shell session
    "unsetenv", // Removes an environment variable from the shell
    "theme" // Display/set theme of shell
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
    &unset_env_var, // builtin_command_func[6]: unsetenv
    &set_theme // builtin_command_func[7]: theme
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
    if(args[1] == NULL) {
        printf("Command not given. Type usage <command>.\n");
        return 1;
    }
    else if (strcmp(args[1], "cd") == 0) {
        printf("Type: cd directory_name to change the current working directory of the shell\n");
        return 1;
    }
    else if (strcmp(args[1], "help") == 0) {
        printf("Type: help for supported commands\n");
        return 1;
    }
    else if (strcmp(args[1], "exit") == 0) {
        printf("Type: exit to terminate the shell gracefully\n");
        return 1;
    }
    else if (strcmp(args[1], "usage") == 0) {
        printf("Type: usage cd/help/exit\n");
        return 1;
    }
    else if (strcmp(args[1], "env") == 0) {
        printf("Type: env to list all registered env variables\n");
        return 1;
    }
    else if (strcmp(args[1], "setenv") == 0) {
        printf("Type: setenv ENV=VALUE to set a new env variable\n");
        return 1;
    }
    else if (strcmp(args[1], "unsetenv") == 0) {
        printf("Type: unsetenv ENV to remove this env from the list of env variables\n");
        return 1;
    }
    else if (strcmp(args[1], "theme") == 0) {
        printf("Type: theme THEME to change the shell's theme, or simply \"theme\" to display current theme\n");
        return 1;
    }
    else {
        printf("The command you gave: %s, is not part of CSEShell's builtin commands.\n", args[1]);
    }
    return 1;
}

int list_env(char **args) {
    char **env = environ; // Pointer to the array of environment strings

    while (*env) {
        printf("%s\n", *env);
        env++;
    }
    return 1;
}

// Add to list of process' environment variables
// Use setenv() instead of putenv() since cmd will be freed and may cause issues
int set_env_var(char **args) {
    // First make sure that there are arguments provided
    if (args[1] == NULL) {
        printf("Invalid arguments provided. \n");
        printf("Syntax: setenv key=value\n");
        printf("Example usage: setenv name=\"Indodon\"\n");
        return -1;
    }

    // Create a new char array since strtok modifies in-place
    char env_var[MAX_LINE];
    strcpy(env_var, args[1]);

    // split the first argument into key & val (= delimiter)
    char *key = strtok(env_var, "=");
    char *val = strtok(NULL, "=");
    if (key == NULL || val == NULL) {
        printf("Invalid arguments provided. \n");
        printf("Syntax: setenv key=value\n");
        printf("Example usage: setenv name=\"Indodon\"\n");
        return -1;
    }

    int status = setenv(key, val, 1);
    if (status) {
        printf("setenv failed. \n");
    }

    return status ? -1 : 1;
}

// Remove from list of process' environment variables
int unset_env_var(char **args) {
    // First make sure that there are arguments provided
    if (args[1] == NULL) {
        printf("Invalid arguments provided. \n");
        printf("Syntax: unsetenv key\n");
        printf("Example usage: unsetenv name\n");
        return -1;
    }

    int status = unsetenv(args[1]);
    if (status) {
        printf("unsetenv failed. \n");
        printf("Syntax: unsetenv key\n");
        printf("Example usage: unsetenv name\n");
    }
    
    return status ? -1 : 1;
}

// Set/display theme of shell
int set_theme(char **args) {
    if (args[1] == NULL) {
        printf("Current theme: %s \n", "PLACEHOLDER");
        printf("Syntax: theme [default | colorblind | contrast | cyberpunk | monochrome]\n");
        printf("Example usage: theme default\n");
        return 1;
    }
    if (strcmp(args[1], "default") == 0) {
        theme_banner_1 = COLOR_BRIGHT_RED;
        theme_banner_2 = COLOR_BRIGHT_WHITE;
        theme_userhost = COLOR_BRIGHT_GREEN;
        theme_cwd = COLOR_BRIGHT_BLUE;
        theme_prompt = COLOR_BRIGHT_BLACK ;
    }
    else if (strcmp(args[1], "colorblind") == 0) {
        theme_banner_1 = COLOR_BRIGHT_CYAN;
        theme_banner_2 = COLOR_BRIGHT_CYAN;
        theme_userhost = COLOR_BRIGHT_YELLOW;
        theme_cwd = COLOR_BRIGHT_WHITE;
        theme_prompt = COLOR_CYAN;
    }
    else if (strcmp(args[1], "contrast") == 0) {
        theme_banner_1 = COLOR_WHITE;
        theme_banner_2 = COLOR_WHITE;
        theme_userhost = COLOR_BRIGHT_BLUE;
        theme_cwd = COLOR_BRIGHT_CYAN;
        theme_prompt = COLOR_BRIGHT_WHITE;
    }
    else if (strcmp(args[1], "cyberpunk") == 0) {
        theme_banner_1 = COLOR_BRIGHT_MAGENTA;
        theme_banner_2 = COLOR_BRIGHT_MAGENTA;
        theme_userhost = COLOR_BRIGHT_CYAN;
        theme_cwd = COLOR_YELLOW;
        theme_prompt = COLOR_MAGENTA;
    }
    else if (strcmp(args[1], "monochrome") == 0) {
        theme_banner_1 = COLOR_BRIGHT_GREEN;
        theme_banner_2 = COLOR_BRIGHT_GREEN;
        theme_userhost = COLOR_GREEN;
        theme_cwd = COLOR_BRIGHT_WHITE;
        theme_prompt = COLOR_BRIGHT_BLACK;
    }
    else if (strcmp(args[1], "white") == 0) {
        theme_banner_1 = COLOR_BRIGHT_WHITE;
        theme_banner_2 = COLOR_BRIGHT_WHITE;
        theme_userhost = COLOR_BRIGHT_WHITE;
        theme_cwd = COLOR_BRIGHT_WHITE;
        theme_prompt = COLOR_WHITE;
    }
    else if (strcmp(args[1], "green") == 0) {
        theme_banner_1 = COLOR_BRIGHT_GREEN;
        theme_banner_2 = COLOR_BRIGHT_GREEN;
        theme_userhost = COLOR_BRIGHT_GREEN;
        theme_cwd = COLOR_BRIGHT_GREEN;
        theme_prompt = COLOR_GREEN ;
    }
    else {
        printf("Syntax: theme [default | colorblind | contrast | cyberpunk | monochrome | white | green]\n");
        printf("Example usage: theme default\n");
        return -1;
    }
    print_ascii();
    return 1;
}