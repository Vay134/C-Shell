#include "shell.h"

// The main function where the shell's execution begins
int main(void)
{
    // Define an array to hold the command and its arguments
    char *cmd[MAX_ARGS]; // Array of pointers to characters
    int child_status;
    pid_t pid;

    while (1)
    {                      // Infinite loop
        type_prompt();     // Display the prompt
        read_command(cmd); // Read a command from the user

        // If the command is "exit", break out of the loop to terminate the shell
        if (strcmp(cmd[0], "exit") == 0)
            // break;
            return 0;

        if (cmd[0] == NULL)
            // empty command, skip execution
            continue;

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
            // Formulate the full path of the command to be executed
            char full_path[PATH_MAX];
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {

                snprintf(full_path, sizeof(full_path), "%s/bin/%s", cwd, cmd[0]);
            }
            else
            {
                printf("Failed to get current working directory.");
                exit(1);
            }

            execv(full_path, cmd);

            // If execv returns, command execution has failed
            printf("Command %s not found\n", cmd[0]);
            exit(0);
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

            // Free the allocated memory for the command arguments before exiting
            for (int i = 0; cmd[i] != NULL; i++)
            {
                free(cmd[i]);
            }
        }
    }
}