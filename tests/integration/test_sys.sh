#!/usr/bin/env bash
# tests/integration/test_sys.sh
#
# Verifies that the `sys` system program runs through the shell and prints
# the expected information fields. Values are machine-specific (and may be
# "(unavailable)"), so we only assert that each labelled line is present.

set -euo pipefail

OUTPUT=$(printf "sys\nexit\n" | timeout 5s ./cseshell)

MISSING=""
for label in "OS:" "Kernel:" "Hostname:" "Uptime:" "User:" "CPU:" "MemTotal:"; do
  if ! echo "$OUTPUT" | grep -F "$label" > /dev/null; then
    MISSING="$MISSING $label"
  fi
done

if [[ -n "$MISSING" ]]; then
  echo "FAIL: sys did not print these fields:$MISSING"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

echo "PASS: sys prints all expected information fields"
