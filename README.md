# CSEShell

A small interactive UNIX shell with a set of built-in commands and bundled
system programs.

## Compiling and Running

### Compile

From the repository root, build the shell and all system programs with:

```bash
make
```

This compiles every `.c` file under `source/` (plus `source/libs/`) into the
`cseshell` executable, and builds each program in `source/system_programs/`
into the `bin/` directory.

### Run

```bash
./cseshell
```

On start-up the shell:
1. Prints an ASCII banner (`init_display`).
2. Reads `.cseshellrc` in the current directory (`run_rc`) where
   `PATH` is set so the bundled system programs in `bin/` can be found.
3. Drops into the interactive prompt, which shows the time, `user@host`, and
   the current working directory.

At the prompt you can run any built-in command (below) or any of the system
programs (`find`, `ld`, `ldr`, `sys`, `dspawn`, `dcheck`, `backup`).

### Clean

```bash
make clean
```

> **Note:** the system programs are only found because `.cseshellrc` adds
> `bin/` to `PATH`. If project is moved, update the absolute path in
> `.cseshellrc`.

## Built-in Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `cd` | `cd <dir>` | Change the shell's current working directory. |
| `help` | `help` | List all built-in commands. |
| `exit` | `exit` | Exit the shell gracefully. |
| `usage` | `usage <command>` | Print a short usage guide for a built-in command. |
| `env` | `env` | List all current environment variables. |
| `setenv` | `setenv KEY=VALUE` | Set or modify an environment variable for this session. |
| `unsetenv` | `unsetenv KEY` | Remove an environment variable. |
| `theme` | `theme <name>` | Change the shell's colour theme or display the current theme. |

Available themes for `theme <name>`: `default`, `colorblind`, `contrast`,
`cyberpunk`, `monochrome`, `white`, `green`. Running `theme` with no argument
prints the list.

## System Programs

Built into `bin/` and runnable by name from inside the shell:

- `find` — search for files in a directory.
- `ld` — list the contents of the current directory.
- `ldr` — list the contents of the current directory recursively.
- `sys` — display system information (OS, kernel, hostname, uptime, user, CPU, memory).
- `dspawn` — spawn a detached daemon that logs to `dspawn.log`.
- `dcheck` — report how many `dspawn` daemons are currently running.
- `backup` — zip the directory named by `$BACKUP_DIR` into `./archive/` (requires `zip`).

## Testing

The project ships with two layers of tests.

### Run everything

```bash
make test          # unit tests + integration tests
```

### Unit tests

C programs in `tests/unit/` that exercise pure helper functions directly using
the [Unity](tests/unity) framework.

```bash
make unit
```

Supported unit tests:

| File | Covers |
|------|--------|
| `tests/unit/test_perms.c` | `perms_to_string` (`source/libs/perms.c`); permission bits → `ls`-style string. |
| `tests/unit/test_shell.c` | `split_command` and `clean_arr` (`source/shell.c`); command tokenising and cleanup. |

### Integration tests

Bash scripts in `tests/integration/` that launch the compiled `./cseshell`,
feed it commands on `stdin`, and assert on its output. Requires that `make`
has been run first so `cseshell` and the `bin/` programs exist.

```bash
make integration
```

Supported integration tests:

| Script | What it checks |
|--------|----------------|
| `test_builtin_cd.sh` | `cd` changes the working directory. |
| `test_builtin_env.sh` | `env` / `setenv` / `unsetenv` work together. |
| `test_builtin_help.sh` | `help` lists all built-ins. |
| `test_builtin_usage.sh` | `usage` handles known, missing, and unknown commands. |
| `test_exit.sh` | `exit` terminates the shell. |
| `test_loop.sh` | the shell loops and survives unknown commands. |
| `test_sys.sh` | `sys` prints all expected information fields. |
| `test_dspawn.sh` | `dspawn` starts a daemon that writes to `dspawn.log`. |
| `test_dcheck.sh` | `dcheck` reports at least one live daemon after `dspawn`. |
| `test_backup.sh` | `backup` zips `$BACKUP_DIR` into `./archive/` (skips if `zip` is absent). |
| `test_system_programs_bundled.sh` | `ld`, `find`, and `ldr` work through the shell. |

## Additional Features

- ASCII art banner on shell start.
- Several selectable colour themes, configurable on startup in the .cseshellrc file.
- Time, user, and cwd shown on each prompt.
- Resource-usage reporting for each command that runs.
- .cseshellrc parser that allows parsing environment variables (including PATH), commands, and comments

## Sustainability and Inclusivity

- **Sustainability:** resource-usage reports for each process execution help
  users notice long, resource-intensive commands that consume more energy.
- **Inclusivity:** custom themes let users tailor the shell to their needs,
  including support for colour blindness and light sensitivity.
