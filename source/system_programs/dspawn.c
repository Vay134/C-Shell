#include "system_program.h"

char output_file_path[PATH_MAX];

static int daemon_work()
{
    int num = 0;
    FILE *fptr;
    char *cwd;
    char buffer[1024];

    // write PID of daemon in the beginning
    fptr = fopen(output_file_path, "a");
    if (fptr == NULL)
    {
        return EXIT_FAILURE;
    }

    fprintf(fptr, "Daemon process running with PID: %d, PPID: %d, opening logfile with FD %d\n", getpid(), getppid(), fileno(fptr));

    // then write cwd
    cwd = getcwd(buffer, sizeof(buffer));
    if (cwd == NULL)
    {
        perror("getcwd() error");
        return 1;
    }

    fprintf(fptr, "Current working directory: %s\n", cwd);
    fclose(fptr);

    while (1)
    {

        // use appropriate location if you are using MacOS or Linux
        fptr = fopen(output_file_path, "a");

        if (fptr == NULL)
        {
            return EXIT_FAILURE;
        }

        fprintf(fptr, "PID %d Daemon writing line %d to the file.  \n", getpid(), num);
        num++;

        fclose(fptr);

        sleep(10);

        if (num == 10) // we just let this process terminate after 10 counts
            break;
    }

    return EXIT_SUCCESS;
}

int execute(char **args)
{
    // Store cwd for later
    if (getcwd(output_file_path, sizeof(output_file_path)) == NULL)
    {
        perror("getcwd() error, exiting now.");
        return EXIT_FAILURE;
    }
    strcat(output_file_path, "/dspawn.log"); 

    pid_t pid;
    pid = fork();

    // Fork failure
    if (pid < 0)
    {
        printf("Failed to fork. \n");
        return EXIT_FAILURE;
    }
    // Parent process
    else if (pid != 0)
    {
        return EXIT_SUCCESS;
    }
    // Child process
    // Create new session with the child process at its leader
    // setsid() only works on non-group leaders (e.g. current child process)
    pid_t sid = setsid();

    // Ignore SIGCHLD so parent doesn't have to read exit status
    // Prevents child process from becoming zombie process
    signal(SIGCHLD, SIG_IGN);
    // Ignore SIGHUP to prevent killing children when session leader terminatres
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid > 0)
        return EXIT_SUCCESS;

    // Set permissions to 0777 (- rwx rwx rwx/global RW & executable)
    // umask = mask & 0777, actual_perms = req_perms & (~umask)
    // Daemon has to be global RW & executable since it is detached from shell
    umask(0);
    // chdir to root, prevents daemon from being unmounted while running
    // root will never be unmounted
    chdir("/");

    // Close all open file descriptors (starting from largest)
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
    {
        close(x);
    }

    // Attach file descriptors 0, 1, and 2 to /dev/null.
    // Prevents code referring to these fd from failing
    // Prevents fd from being reused when open() is called from daemon's code
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);

    return daemon_work();;
}

int main(int argc, char **args)
{
    (void)argc;
    return execute(args);
}