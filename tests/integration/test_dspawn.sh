#!/usr/bin/env bash
# tests/integration/test_dspawn.sh
#
# Verifies that `dspawn` launches a detached daemon that writes its startup
# banner to ./dspawn.log in the directory the shell was started from.
#
# We do NOT assert on the daemon's later periodic writes (one every 10s);
# we only check that the daemon came up and wrote its first line.

set -euo pipefail

LOG="./dspawn.log"

cleanup() {
  pkill -x dspawn 2>/dev/null || true   # stop the detached daemon(s)
  rm -f "$LOG"
}
trap cleanup EXIT

rm -f "$LOG"

printf "dspawn\nexit\n" | timeout 5s ./cseshell > /dev/null 2>&1

# Give the freshly detached daemon a moment to open the log and write line 1.
sleep 1

if [[ ! -f "$LOG" ]]; then
  echo "FAIL: dspawn did not create $LOG"
  exit 1
fi

if ! grep -F "Daemon process running with PID" "$LOG" > /dev/null; then
  echo "FAIL: $LOG does not contain the daemon startup banner"
  echo "----- $LOG -----"
  cat "$LOG"
  echo "----------------"
  exit 1
fi

echo "PASS: dspawn starts a daemon that writes to dspawn.log"
