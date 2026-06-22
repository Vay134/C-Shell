#!/usr/bin/env bash
# tests/integration/test_dcheck.sh
#
# Verifies that `dcheck` reports the number of live dspawn daemons.
# We first start a daemon with `dspawn`, then run `dcheck` and assert that
# it prints "Live daemons:" with a count of at least 1.

set -euo pipefail

cleanup() {
  pkill -x dspawn 2>/dev/null || true
  rm -f ./dspawn.log
}
trap cleanup EXIT

rm -f ./dspawn.log

# Start a daemon, then let it finish detaching before we count.
printf "dspawn\nexit\n" | timeout 5s ./cseshell > /dev/null 2>&1
sleep 1

OUTPUT=$(printf "dcheck\nexit\n" | timeout 5s ./cseshell)

if ! echo "$OUTPUT" | grep -F "Live daemons:" > /dev/null; then
  echo "FAIL: dcheck did not print a 'Live daemons:' line"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

COUNT=$(echo "$OUTPUT" | grep -oE "Live daemons:[[:space:]]*[0-9]+" | grep -oE "[0-9]+" | head -n1)

if [[ -z "$COUNT" || "$COUNT" -lt 1 ]]; then
  echo "FAIL: dcheck reported '$COUNT' live daemons, expected at least 1"
  echo "----- shell output -----"
  echo "$OUTPUT"
  echo "------------------------"
  exit 1
fi

echo "PASS: dcheck reports $COUNT live daemon(s) after dspawn"
