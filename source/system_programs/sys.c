#include "system_program.h"
/*
 Print basic information about the operating system:
 OS, Kernel, Hostname, Uptime, User, CPU, MemTotal.
*/

/*
 Open `path`, find the first line beginning with `key`, and copy the value that
 follows into `out`. `sep` is the character between the key and value ('=' or ':').
 Returns 1 if the key was found, 0 otherwise (including if the file won't open).
*/
static int read_field(const char *path, const char *key, char sep, char *out, size_t out_size)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return 0;

    char line[SHELL_BUFFERSIZE];
    size_t key_len = strlen(key);
    int found = 0;

    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, key, key_len) != 0)
            continue;

        char *value = line + key_len;
        while (*value == sep || *value == ' ' || *value == '\t') // Skip the separator and any whitespace padding after it.
            value++;

        value[strcspn(value, "\n")] = '\0'; // replace \n with \0

        size_t len = strlen(value);
        if (len >= 2 && value[0] == '"' && value[len - 1] == '"')
        {
            value[len - 1] = '\0';
            value++;
        }

        strncpy(out, value, out_size - 1);
        out[out_size - 1] = '\0'; // guarantee NUL-termination
        found = 1;
        break;
    }

    fclose(f);
    return found;
}

// Read seconds-since-boot from /proc/uptime and print it as days/hours/minutes.
static void print_uptime(void)
{
    FILE *f = fopen("/proc/uptime", "r");
    double seconds = 0;

    if (!f || fscanf(f, "%lf", &seconds) != 1)
    {
        printf("Uptime:   (unavailable)\n");
        if (f)
            fclose(f);
        return;
    }
    fclose(f);

    long total = (long)seconds;
    long days = total / 86400;          // 86400 seconds in a day
    long hours = (total % 86400) / 3600; // 3600 seconds in an hour
    long minutes = (total % 3600) / 60;

    printf("Uptime:   %ld days, %ld hours, %ld minutes\n", days, hours, minutes);
}

int execute(char **args)
{
    (void)args; 

    struct utsname uts;
    char buf[SHELL_BUFFERSIZE];

    // OS: the friendly name from /etc/os-release, or the kernel name as fallback.
    if (read_field("/etc/os-release", "PRETTY_NAME", '=', buf, sizeof(buf)))
        printf("OS:       %s\n", buf);
    else if (uname(&uts) == 0)
        printf("OS:       %s\n", uts.sysname);
    else
        printf("OS:       (unavailable)\n");

    if (uname(&uts) == 0)
    {
        printf("Kernel:   %s %s\n", uts.sysname, uts.release);
        printf("Hostname: %s\n", uts.nodename);
    }
    else
    {
        printf("Kernel:   (unavailable)\n");
        printf("Hostname: (unavailable)\n");
    }

    print_uptime();

    // look up our numeric UID in the password database. A more concrete way to get username
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_name)
        printf("User:     %s\n", pw->pw_name);
    else
        printf("User:     (unavailable)\n");

    if (read_field("/proc/cpuinfo", "model name", ':', buf, sizeof(buf)))
        printf("CPU:      %s\n", buf);
    else
        printf("CPU:      (unavailable)\n");

    if (read_field("/proc/meminfo", "MemTotal", ':', buf, sizeof(buf)))
        printf("MemTotal: %s\n", buf);
    else
        printf("MemTotal: (unavailable)\n");

    return EXIT_SUCCESS;
}

int main(int argc, char **args)
{
    (void)argc;
    return execute(args);
}
