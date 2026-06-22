[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/CBJ_yXMW)
# CSEShell

## Building the Project

To build the CSEShell and system programs, run the following command in the root directory:

```bash
make
```

This will compile the source code and place the executable files in the appropriate directories.

## Running CSEShell

After building, you can start the shell by running:

```bash
./cseshell
```

From there, you can execute built-in commands and any of the included system programs (e.g., `find`, `ld`, `ldr`).

## Built-in commands

- `shell_cd` - Changes current working directory
- `shell_help` - Lists down all built-in commands
- `shell_exit` - Exits the shell
- `shell_usage` - Displays information regarding built-in commands
- `list_env` - Lists down all the environment variables
- `set_env_var` - Sets an environment variables
- `unset_env_var` - Unsets a specific environment variable
- `set_theme` - Sets the color theme of the shell

## System Programs

- `find.c` - Searches for files in a directory.
- `ld.c` - List the contents of the curent directory.
- `ldr.c` - List the contents of the current directory recursively.
- `sys.c` - Displays information about the system. 
- `dspawn.c` - Spawns a Daemon that counts to 10. 
- `dcheck.c` - Displays how many Daemons are running. 

## Makefile

The Makefile contains rules for compiling the shell and system programs. You can clean the build by running:

```bash
make clean
```

## Testing

This project ships with two layers of tests:

- **Unit tests** in `tests/unit/`. Small C programs that exercise pure helper functions directly, using the Unity framework. You can create any matching `test_foo.c` under `tests/unit` to test any libs under `source/libs/foo.c` by including the matching `source/libs/foo.h` header file in the unit test. See `tests/unit/test_perms.c` (or `test_rc_parser.c`) for example.
- **Integration tests** in `tests/integration/`. Bash scripts that run the compiled `./cseshell` as a subprocess, feed it `stdin`, and check `stdout`.
  - You should create your own integration tests. These samples given are just samples, adjust it accordingly.

Run all tests:

```bash
make test
```

Run only unit tests:

```bash
make unit
```

Run only integration tests (requires that `make` has been run first so `cseshell` and the system program binaries exist):

```bash
make integration
```

For an explanation of what to test and how to structure your testable code, see the testing handout.

## Additional features: 
- ASCII art on shell start
- Several color themes
- Time, user, cwd display on each prompt
- Resource usage reports for every individual command ran

## Sustainability and Inclusivity: 
- Sustainability: We added resource usage reports for each process execution so that users of our CSEShell can be more mindful of long, resource-intensive processes that require plenty of energy. 
- Inclusivity: By implementing custom themes, our programmes allow for users with specific preferences to customize the shell. This includes individuals with specific needs such as color blindness and light sensitivity. 