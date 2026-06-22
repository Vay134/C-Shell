// This code was written by ChatGPT4
// Modify it for your own usage to implement features for PA1 (or completely
// rewrite it) Include the shell header file for necessary constants and
// function declarations
#include "shell.h"

void init_display() {
    // Clear the screen on the first call
    #ifdef _WIN32
    system("cls"); // Windows command to clear screen
    #else
    system("clear"); // UNIX/Linux command to clear screen
    #endif

    printf(
        COLOR_BRIGHT_RED "              _   _    ____      U  ___ u  ____      U  ___ u  _   _     \n"
        "     ___     | \\ |\"|  |  _\"\\      \\/\"_ \\/ |  _\"\\      \\/\"_ \\/ | \\ |\"|    \n"
        "    |_\"_|   <|  \\| |>/| | | |     | | | |/| | | |     | | | |<|  \\| |>   \n"
        "     | |   " COLOR_BRIGHT_WHITE " U| |\\  |uU| |_| |\\.-,_| |_| |U| |_| |\\.-,_| |_| |U| |\\  |u   \n"
        "   U/| |\\u   |_| \\_|  |____/ u \\_)-\\___/  |____/ u \\_)-\\___/  |_| \\_|    \n"
        ".-,_|___|_,-.||   \\\\,-.|||_         \\\\     |||_         \\\\    ||   \\\\,-. \n"
        " \\_)-' '-(_/ (_\")  (_/(__)_)       (__)   (__)_)       (__)   (_\")  (_/  \n\n" COLOR_RESET);
}

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

// Function to read a command from the user input
int read_command(char **cmd)
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

    // If only the newline character was entered, return without processing
    if (count == 1)
        return -1;

    split_command(cmd, line);
    return 0;
}

int get_user_host(char *hostname, char* user) {
    if (gethostname(hostname, HOST_NAME_MAX + 1) != 0) {
        perror("gethostname failed");
        return 1;
    }

    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL) {
        perror("getpwuid failed");
        return 1;
    }
    strcpy(user, pw->pw_name);
    return 0;
}

// Function to display the shell prompt
void type_prompt()
{
    fflush(stdout); // Flush the output buffer
    char cwd[1024];

    char hostname[HOST_NAME_MAX + 1], user[LOGIN_NAME_MAX + 1];
    if (get_user_host(hostname, user) == 0) {
        printf(COLOR_BRIGHT_GREEN "%s@%s:" COLOR_RESET, user, hostname);
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf(COLOR_BRIGHT_BLUE "%s \n" COLOR_RESET, cwd); // Print current working directory
    }
    printf(COLOR_BRIGHT_BLACK " → "); // Print the shell prompt
}

// Function to clean an array back to its unmodified state
void clean_arr(char **arr)
{
    for (int i = 0; arr[i] != NULL; i++)
    {
        free(arr[i]);
        // To prevent dangling pointers, update each pointer to NULL
        arr[i] = NULL;
    }
}

// Function to run the .rc file
// Should be called
void run_rc(char **cmd, int child_status, pid_t pid)
{
    char rcPath[PATH_MAX];
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        snprintf(rcPath, sizeof(rcPath), "%s/.cseshellrc", cwd, cmd[0]);
    }
    else
    {
        // return EXIT_FAILURE;
        return;
    }

    FILE *rc = fopen(rcPath, "r");
    if (rc == NULL)
    {
        perror("Failed to open .cseshellrc! \n");
        // return EXIT_FAILURE;
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), rc))
    {
        // Replace the newline of each line with null terminator if it's there
        if (strlen(line) > 0 && line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        // Check if the first character is a null terminator
        if (line[0] == '\0')
        {
            continue;
        }
        // Check if the first 4 characters in line are "PATH"
        // strncmp only compares up to the first n characters
        else if (strncmp(line, "PATH", 4) == 0)
        {
            // Set PATH
            // split the first argument into key & val (= delimiter)
            char *key = strtok(line, "=");
            char *val = strtok(NULL, "=");
            if (key == NULL || val == NULL)
            {
                printf("PATH declaration failed. \n");
                continue;
            }

            int status = setenv(key, val, 1);
            if (status)
            {
                printf("setenv failed. \n");
            }

            continue;
        }
        else
        {
            // run command
            char *cmd[MAX_ARGS]; // Array of pointers to characters
            split_command(cmd, line);

            // Fork the current process.
            // Parent should wait for the child to finish.
            // Child should be the one handling command execution.
            pid = fork();

            if (pid < 0)
            {
                // Fork failure
                printf("Failed to fork.");
            }
            else if (pid == 0)
            {
                // Child process
                // Execute file in PATH directory
                execvp(cmd[0], cmd);

                // If execvp returns, command execution has failed
                printf("Command %s not found\n", cmd[0]);
                _exit(0);
            }
            else
            {
                waitpid(pid, &child_status, WUNTRACED);

                if (WIFEXITED(child_status))
                {
                    // Child was terminated normally
                    printf("Child exited with status: %d\n", WEXITSTATUS(child_status));
                }
                else
                {
                    // Child didn't terminate normally
                    printf("Child didn't terminate regularly. \n");
                }

                // Clean cmd for the next loop
                clean_arr(cmd);
            }
        }
    }
    fclose(rc);
}