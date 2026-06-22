#include "system_program.h"

int execute(char **args) {
    system("ps -efj | grep dspawn  | grep -Ev 'tty|pts'");
    return 0;
}

int main(int argc, char **args)
{
    (void)argc;
    return execute(args);
}