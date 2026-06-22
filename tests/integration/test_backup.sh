#!/usr/bin/env bash
# tests/integration/test_backup.sh
#
# Verifies that `backup` reads BACKUP_DIR from the environment (set via the
# shell's setenv builtin) and produces a timestamped zip under ./archive/.
#
# Requires the `zip` utility. If it is not installed the test is skipped.

set -euo pipefail

if ! command -v zip > /dev/null 2>&1; then
  echo "SKIP: zip is not installed; cannot test backup"
  exit 0
fi

FIXTURE=$(mktemp -d)
BASE=$(basename "$FIXTURE")
echo "payload" > "$FIXTURE/sample.txt"

cleanup() {
  rm -rf "$FIXTURE"
  rm -f ./archive/"${BASE}"_*.zip
  rmdir ./archive 2>/dev/null || true   # only removes archive/ if now empty
}
trap cleanup EXIT

OUTPUT=$(printf "setenv BACKUP_DIR=%s\nbackup\nexit\n" "$FIXTURE" | timeout 10s ./cseshell 2>&1)

if ! echo "$OUTPUT" | grep -F "Backup successful" > /dev/null; then
  echo "FAIL: backup did not report success"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

# The zip name embeds a timestamp we cannot predict, so match by prefix.
if ! ls ./archive/"${BASE}"_*.zip > /dev/null 2>&1; then
  echo "FAIL: no archive/${BASE}_*.zip was created"
  echo "----- archive contents -----"
  ls -la ./archive 2>&1 || true
  echo "----------------------------"
  exit 1
fi

echo "PASS: backup zips BACKUP_DIR into ./archive/"
