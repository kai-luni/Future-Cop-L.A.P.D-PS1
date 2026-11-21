#!/bin/sh
# QEMU wrapper that records every executed guest block per test binary.
#
# The host test scripts call "$QEMU <binary> <args...>". Pointing QEMU at this
# wrapper makes every test run normally *and* drop a trace next to it, so
# coverage can be measured with the real fixtures in place - there is no need to
# reproduce each script's fixture setup in Python.
#
# qemu writes the -d in_asm log to stderr and the program's output to stdout, so
# a single run keeps both: stdout passes through to the test script untouched.
#
# Usage:  QEMU=tests/host/qemu_trace.sh bash tests/host/<name>_test.sh
set -e

REAL="${QEMU_REAL:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
OUT="${QEMU_TRACE_DIR:-/tmp/qemu-traces}"
mkdir -p "$OUT"

NAME=$(basename "$1")
TRACE="$OUT/$NAME.in_asm"

# Appending keeps every run of the same binary (different fixtures) in one trace.
"$REAL" -d in_asm "$@" 2>>"$TRACE"
