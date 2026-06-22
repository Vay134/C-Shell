#include "shell.h"

// The main function where the shell's execution begins
int main(void)
{
    // Initialize shell colors
    init_colors();

    // Initialize display of shell
    init_display();

    // Parse & run the RC file
    run_rc();

    // Call the main loop
    return shell_loop();
}