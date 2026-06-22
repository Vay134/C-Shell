#include "shell.h"

// The main function where the shell's execution begins
int main(void)
{
    // Initialize display of shell
    init_display();

    // Parse & run the RC file
    run_rc();

    // Initialize shell colors
    init_colors();

    // Call the main loop
    return shell_loop();
}