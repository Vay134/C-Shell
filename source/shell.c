#include "shell.h"

// Define color themes
char *theme_banner_1;
char *theme_banner_2;
char *theme_userhost;
char *theme_cwd;
char *theme_prompt;

// Function to print ASCII art
void print_ascii()
{
    printf(
        "%s              _   _    ____      U  ___ u  ____      U  ___ u  _   _     \n"
        "     ___     | \\ |\"|  |  _\"\\      \\/\"_ \\/ |  _\"\\      \\/\"_ \\/ | \\ |\"|    \n"
        "    |_\"_|   <|  \\| |>/| | | |     | | | |/| | | |     | | | |<|  \\| |>   \n"
        "     | |   %s U| |\\  |uU| |_| |\\.-,_| |_| |U| |_| |\\.-,_| |_| |U| |\\  |u   \n"
        "   U/| |\\u   |_| \\_|  |____/ u \\_)-\\___/  |____/ u \\_)-\\___/  |_| \\_|    \n"
        ".-,_|___|_,-.||   \\\\,-.|||_         \\\\     |||_         \\\\    ||   \\\\,-. \n"
        " \\_)-' '-(_/ (_\")  (_/(__)_)       (__)   (__)_)       (__)   (_\")  (_/  \n\n" COLOR_RESET,
        theme_banner_1, theme_banner_2);
}

void init_display()
{
// Clear the screen on the first call
#ifdef _WIN32
    system("cls"); // Windows command to clear screen
#else
    system("clear"); // UNIX/Linux command to clear screen
#endif
}

// Helper function to split a line into an array of its arguments (cmd)
void split_command(char **cmd, char *line)
{
    int i = 0;
    char *command_token;
    // Array to hold pointers to the parsed command arguments
    char *array[MAX_ARGS];

    // Use strtok to parse the first token (word) of the command
    command_token = strtok(line, " \n");

    // Continue parsing the line into words and store them in the array
    while (command_token != NULL)
    {
        array[i++] = strdup(command_token);  // Duplicate the token and store it
        command_token = strtok(NULL, " \n"); // Get the next token
    }

    // Copy the parsed command and its parameters to the cmd array
    for (int j = 0; j < i; j++)
    {
        cmd[j] = array[j];
    }
    // Null-terminate the cmd array to mark the end of arguments
    cmd[i] = NULL;
}

// Function to set initial colors to default
void init_colors()
{
    char *cmd[MAX_ARGS]; // Array of pointers to characters
    char *theme = getenv("THEME");
    char line[MAX_LINE];
    if (theme == NULL)
    {
        snprintf(line, sizeof(line), "theme default");
    }
    else
    {
        snprintf(line, sizeof(line), "theme %s", theme);
    }
    split_command(cmd, line);
    set_theme(cmd);
}

// Helper function to read a command from the user input
static int read_command(char **cmd)
{
    // Define a character array to store the command line input
    char line[MAX_LINE];
    // Initialize count to keep track of the number of characters read
    int count = 0;

    // Infinite loop to read characters until a newline or maximum line length is
    // reached
    for (;;)
    {
        // Read a single character from standard input
        int current_char = fgetc(stdin);
        // Store the character in the line array and increment count
        line[count++] = (char)current_char;
        // If a newline character is encountered, break out of the loop
        if (current_char == '\n')
            break;
        // If the command exceeds the maximum length, print an error and exit
        if (count >= MAX_LINE)
        {
            printf("Command is too long, unable to process\n");
            return -1;
        }
    }
    // Null-terminate the command line string
    line[count] = '\0';

    // Explicitly set cmd[0] as NULL for easy checking of "\n" line
    cmd[0] = NULL;

    // If only the newline character was entered, return without processing
    if (count == 1)
        return -1;

    split_command(cmd, line);
    return 0;
}

// Helper function to clean an array back to its original state
void clean_arr(char **arr)
{
    for (int i = 0; arr[i] != NULL; i++)
    {
        free(arr[i]);
        // To prevent dangling pointers, update each pointer to NULL
        arr[i] = NULL;
    }
}

// Helper function to get user and host
int get_user_host(char *hostname, char *user)
{
    if (gethostname(hostname, HOST_NAME_MAX + 1) != 0)
    {
        perror("gethostname failed");
        return 1;
    }

    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL)
    {
        perror("getpwuid failed");
        return 1;
    }
    strcpy(user, pw->pw_name);
    return 0;
}

static struct tm *get_localtime()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now); // Convert to local time
    return t;
}

// Helper function to display the shell prompt
void type_prompt()
{
    fflush(stdout); // Flush the output buffer
    char cwd[1024];

    char hostname[HOST_NAME_MAX + 1], user[LOGIN_NAME_MAX + 1];

    struct tm *t = get_localtime();
    printf("%d:%d ", t->tm_hour, t->tm_min);

    if (get_user_host(hostname, user) == 0)
    {
        printf(BOLD "%s%s@%s:" COLOR_RESET, theme_userhost, user, hostname);
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf(BOLD "%s%s \n" COLOR_RESET, theme_cwd, cwd); // Print current working directory
    }
    printf("%s → ", theme_prompt); // Print the shell prompt
}

static void display_usage()
{
    struct rusage usage;
    if (getrusage(RUSAGE_CHILDREN, &usage) == 0)
    {
        printf("Command resource usage: \n");
        // sec: seconds, usec: microseconds
        printf("User CPU time: %.4fs\n", usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0);
        printf("System CPU time: %.4fs\n", usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0);
        // usage.ru.maxrss is in KB, divide by 1024 to convert to MB
        printf("Peak memory usage: %.3f MB\n", usage.ru_maxrss / 1024.0);
    }
}

// Helper function to execute system programs
static void exec_sys_prog(char **cmd, int is_rc)
{
    int child_status;
    pid_t pid = fork();

    if (pid < 0)
    {
        // Fork failure
        perror("Failed to fork.\n");
    }
    else if (pid == 0)
    {
        // Child process
        execvp(cmd[0], cmd);

        // If execvp returns, command execution has failed
        printf("Command %s not found\n", cmd[0]);
        fflush(stdout);
        _exit(0);
    }
    else
    {
        // Parent process
        // Wait for child process to stop, store status of child in child_status
        // WUNTRACED flag makes waitpid also return if a child process was irregularly terminated
        waitpid(pid, &child_status, WUNTRACED);

        // WIFEXITED(status) returns true if child is terminated normally (by exit or return),
        // false if otherwise (e.g. crash, killed by signal)
        // Check is required since WEXITSTATUS(status) is only valid for regular terminations
        if (!WIFEXITED(child_status))
        {
            // Child didn't terminate normally
            printf("Child didn't terminate regularly. \n");
        }
        else
        {
            if (!is_rc)
            {
                // Display usage only if execvp not called from rc file parsing
                display_usage();
            }
        }
    }
}

// Helper function to handle each line in rc file
static void handle_rc_line(char **cmd, char *line)
{
    // Replace the newline of each line with null terminator if it's there
    if (strlen(line) > 0 && line[strlen(line) - 1] == '\n')
        line[strlen(line) - 1] = '\0';

    // Create pointer to first char
    char *start = line;
    // skip leading whitespaces
    while (*start == ' ' || *start == '\t')
    {
        start++;
    }

    // Check if the first character is a null terminator or a comment
    if (*start == '\0' || *start == '#')
        return;

    // Check if '=' in the rest of the line
    if (strchr(start, '='))
    {
        // Set environment variable
        // split the first argument into key & val (= delimiter)
        char *key = strtok(line, "=");
        char *val = strtok(NULL, "=");
        if (key == NULL || val == NULL)
        {
            printf("Unable to extract key and value. \n");
            return;
        }

        int status = setenv(key, val, 1);
        if (status)
            printf("setenv failed. \n");

        return;
    }
    else
    {
        // run command
        split_command(cmd, line);
        exec_sys_prog(cmd, 1);
        // Clean cmd for the next loop
        clean_arr(cmd);
    }
}

// Function to run the .rc file
// Should be called only when shell is initialized
int run_rc()
{
    char rcPath[PATH_MAX];
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        snprintf(rcPath, sizeof(rcPath), "%s/.cseshellrc", cwd);
    }
    else
    {
        return EXIT_FAILURE;
    }

    FILE *rc = fopen(rcPath, "r");
    if (rc == NULL)
    {
        perror("Failed to open .cseshellrc! \n");
        return EXIT_FAILURE;
    }

    char *cmd[MAX_ARGS]; // Array of pointers to characters
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), rc))
    {
        handle_rc_line(cmd, line);
    }
    fclose(rc);

    return EXIT_SUCCESS;
}

// Main shell loop
int shell_loop()
{
    // Define an array to hold the command and its arguments
    char *cmd[MAX_ARGS]; // Array of pointers to characters
    int child_status;
    pid_t pid;

    while (1)
    {
        type_prompt(); // Display the prompt
        // Read a command from the user
        if (read_command(cmd) != 0)
        {
            continue;
        }
        printf(COLOR_RESET); // Reset color after being set in type_prompt()
        fflush(stdout);      // Force COLOR_RESET

        // If the command is empty, skip this loop
        if (cmd[0] == NULL)
            continue;

        // Loop through builtin commands to see if cmd[0] matches any of them
        int is_builtin = 0;
        for (int cmd_i = 0; cmd_i < num_builtin_functions(); cmd_i++)
        {
            if (strcmp(cmd[0], builtin_commands[cmd_i]) == 0)
            {
                // Run the builtin command in the shell process
                if ((*builtin_command_func[cmd_i])(cmd) == 0)
                {
                    // Return 0 if cmd[0] is exit
                    return 0;
                }
                is_builtin = 1; // Set the flag to true
                break;          // Stop checking for other cmds
            }
        }

        if (is_builtin)
        {
            clean_arr(cmd); // Reset cmd array
            continue;
        }

        exec_sys_prog(cmd, 0);

        // Clean cmd for the next loop
        clean_arr(cmd);
    }
}