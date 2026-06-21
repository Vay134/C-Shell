#ifndef SHELL_H
#define SHELL_H

#include <limits.h> // For PATH_MAX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define BIN_PATH "./bin/"

void init_display();
void type_prompt();
int read_command(char **cmd);
void clean_arr(char **cmd);

extern const char *builtin_commands[];
int num_builtin_functions();

/*
Handler of each shell builtin function
*/
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_usage(char **args);
int list_env(char **args);
int set_env_var(char **args);
int unset_env_var(char **args);

extern int (*builtin_command_func[])(char **);

// C standard library environ
extern char **environ; 

void run_rc(char **cmd, int child_status, pid_t pid);

#endif