#ifndef SHELL_H
#define SHELL_H

#include <limits.h> // For PATH_MAX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> // For pwd.h
#include <pwd.h> // For getting user

#define MAX_LINE 1024
#define MAX_ARGS 64
#define BIN_PATH "./bin/"

int run_rc();
int shell_loop();
void init_display();

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

// Color & font definitions
#define COLOR_BLACK             "\x1b[30m"
#define COLOR_RED               "\x1b[31m"
#define COLOR_GREEN             "\x1b[32m"
#define COLOR_YELLOW            "\x1b[33m"
#define COLOR_BLUE              "\x1b[34m"
#define COLOR_MAGENTA           "\x1b[35m"
#define COLOR_CYAN              "\x1b[36m"
#define COLOR_WHITE             "\x1b[37m"

#define COLOR_BRIGHT_BLACK      "\x1b[90m"
#define COLOR_BRIGHT_RED        "\x1b[91m"
#define COLOR_BRIGHT_GREEN      "\x1b[92m"
#define COLOR_BRIGHT_YELLOW     "\x1b[93m"
#define COLOR_BRIGHT_BLUE       "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA    "\x1b[95m"
#define COLOR_BRIGHT_CYAN       "\x1b[96m"
#define COLOR_BRIGHT_WHITE      "\x1b[97m"

#define BG_BLACK                "\x1b[40m"
#define BG_RED                  "\x1b[41m"
#define BG_GREEN                "\x1b[42m"
#define BG_YELLOW               "\x1b[43m"
#define BG_BLUE                 "\x1b[44m"
#define BG_MAGENTA              "\x1b[45m"
#define BG_CYAN                 "\x1b[46m"
#define BG_WHITE                "\x1b[47m"

#define BG_BRIGHT_BLACK         "\x1b[100m"
#define BG_BRIGHT_RED           "\x1b[101m"
#define BG_BRIGHT_GREEN         "\x1b[102m"
#define BG_BRIGHT_YELLOW        "\x1b[103m"
#define BG_BRIGHT_BLUE          "\x1b[104m"
#define BG_BRIGHT_MAGENTA       "\x1b[105m"
#define BG_BRIGHT_CYAN          "\x1b[106m"
#define BG_BRIGHT_WHITE         "\x1b[107m"

#define BOLD                    "\x1b[1m"
#define COLOR_RESET             "\x1b[0m"

#endif