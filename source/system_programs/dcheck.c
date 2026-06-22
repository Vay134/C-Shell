#include "system_program.h"

int execute(char **args) {
    // ps: process status
    //  -e: every process, regardless of ownership
    //  -f: full details
    //  -j: shows the SID (session ID) and PGID (Process Group ID) as well

    // grep: get items with specific text patterns
    //  -E: enables Extended RegEx
    //  -v: invert selection
    //  'tty|pts': if a process is tied to physical terminal ports (tty) or 
    //             pts (pseudo-terminal slave), they are likely manually launched

    // wc: word count
    //  -l: count lines only
    printf("Live daemons: ");
    fflush(stdout); // forces the printf to be printed before lines below
    system("ps -efj | grep dspawn  | grep -Ev 'tty|pts' | wc -l");
    return EXIT_SUCCESS;
}

int main(int argc, char **args)
{
    (void)argc;
    return execute(args);
}