#!/usr/bin/env bash
# tests/integration/test_builtin_usage.sh
#
# Verifies the `usage` builtin:
#   - usage <known-builtin> prints a "Type:" help line for that command.
#   - usage with no argument prompts for a command.
#   - usage <unknown> reports that the command is not a builtin.

set -euo pipefail

OUTPUT=$(printf "usage env\nusage\nusage notacommand\nexit\n" | timeout 3s ./cseshell)

fail() {
  echo "FAIL: $1"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
}

# usage env -> a help line describing env.
echo "$OUTPUT" | grep -F "Type: env to list all registered env variables" > /dev/null \
  || fail "usage env did not print the env help line"

# usage with no argument -> prompt for a command.
echo "$OUTPUT" | grep -F "Command not given" > /dev/null \
  || fail "usage with no argument did not prompt for a command"

# usage on an unknown command -> not-a-builtin message.
echo "$OUTPUT" | grep -F "is not part of CSEShell's builtin commands" > /dev/null \
  || fail "usage on an unknown command did not report it as a non-builtin"

echo "PASS: usage handles known, missing, and unknown commands"
