#!/bin/sh
# QEMU wrapper that records every executed guest block per test binary.
#
# The host test scripts call "$QEMU <binary> <args...>". Pointing QEMU at this
# wrapper makes every test run normally *and* drop a trace next to it, so
# coverage can be measured with the real fixtures in place - there is no need to
# reproduce each script's fixture setup in Python.
#
# QEMU writes its trace to a separate file. Guest stdout and stderr both pass
# through unchanged, including for tests that check error messages.
#
# Usage:  QEMU=tests/host/qemu_trace.sh bash tests/host/<name>_test.sh
set -e

REAL="${QEMU_REAL:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
OUT="${QEMU_TRACE_DIR:-/tmp/qemu-traces}"
mkdir -p "$OUT"

NAME=$(basename "$1")
KEY=$(printf '%s' "$(realpath -m "$1")" | sha256sum)
KEY=${KEY%% *}
TRACE="$OUT/$NAME-$KEY.in_asm"

# Keep each QEMU invocation separate: QEMU's -D may truncate its output.
PART=$(mktemp "$OUT/.trace.XXXXXX")
trap 'rm -f "$PART"' EXIT HUP INT TERM
set +e
"$REAL" -d in_asm -D "$PART" "$@"
RC=$?
set -e
cat "$PART" >> "$TRACE"
exit "$RC"
