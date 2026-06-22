#include "system_program.h"

int execute(char **args)
{
    (void)args;

    char *backup_dir = getenv("BACKUP_DIR");
    if (backup_dir == NULL)
    {
        fprintf(stderr, COLOR_RED "Error: BACKUP_DIR environment variable is not set\n" COLOR_RESET);
        return EXIT_FAILURE;
    }

    struct stat st;
    if (stat(backup_dir, &st) != 0)
    {
        fprintf(stderr, COLOR_RED "Error: '%s' does not exist: %s\n" COLOR_RESET, backup_dir, strerror(errno));
        return EXIT_FAILURE;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);

    /* Extract base name without modifying backup_dir */
    const char *base = strrchr(backup_dir, '/');
    base = base ? base + 1 : backup_dir;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, COLOR_RED "Error: cannot get working directory: %s\n" COLOR_RESET, strerror(errno));
        return EXIT_FAILURE;
    }

    /* Create ./archive/ if it does not exist */
    char archive_root[PATH_MAX];
    snprintf(archive_root, sizeof(archive_root), "%s/archive", cwd);
    if (mkdir(archive_root, 0755) != 0 && errno != EEXIST)
    {
        fprintf(stderr, COLOR_RED "Error: cannot create archive directory: %s\n" COLOR_RESET, strerror(errno));
        return EXIT_FAILURE;
    }

    /* Destination: ./archive/<basename>_<timestamp>.zip */
    char zip_path[PATH_MAX];
    snprintf(zip_path, sizeof(zip_path), "%s/%s_%s.zip", archive_root, base, timestamp);

    printf(COLOR_CYAN "Backing up " COLOR_YELLOW "'%s'" COLOR_CYAN " -> " COLOR_GREEN "'%s'\n" COLOR_RESET,
           backup_dir, zip_path);

    /* Fork and exec zip */
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, COLOR_RED "Error: fork failed: %s\n" COLOR_RESET, strerror(errno));
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        /* Child */
        if (S_ISDIR(st.st_mode))
        {
            char *zip_args[] = {"zip", "-r", zip_path, backup_dir, NULL};
            execvp("zip", zip_args);
        }
        else
        {
            char *zip_args[] = {"zip", zip_path, backup_dir, NULL};
            execvp("zip", zip_args);
        }
        fprintf(stderr, COLOR_RED "Error: exec zip failed: %s\n" COLOR_RESET, strerror(errno));
        _exit(EXIT_FAILURE);
    }

    /* Parent: wait for zip to finish */
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        printf(COLOR_GREEN "Backup successful: %s\n" COLOR_RESET, zip_path);
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, COLOR_RED "Error: backup failed (zip exited with %d)\n" COLOR_RESET,
                WIFEXITED(status) ? WEXITSTATUS(status) : -1);
        return EXIT_FAILURE;
    }
}

int main(int argc, char **args)
{
    (void)argc;
    return execute(args);
}
