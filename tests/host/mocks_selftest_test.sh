#!/bin/bash
# Self-test for the shared mock library (tests/host/mocks/).
#
# This links the mocks with no game unit at all: it checks the mocks' own
# contract, so a mock that invented a return value or swallowed an unsupported
# call fails here rather than turning some other harness green.
#
# Three runs:
#   1. the contract checks, which must pass;
#   2. an unsupported A0 service, which must abort with exit 2;
#   3. an unknown BIOS table, which must abort with exit 2.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-mocks-selftest}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile() {
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -I"$HERE" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" 2>/dev/null | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}

compile "$HERE/mocks/sound_mock.c" "$OUT/sound_mock.o"
compile "$HERE/mocks/gpu_mock.c" "$OUT/gpu_mock.o"
compile "$HERE/mocks/bios_mock.c" "$OUT/bios_mock.o"
compile "$HERE/mocks_selftest_harness.c" "$OUT/mocks_selftest_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/mocks_selftest" \
    "$OUT/crt0.o" "$OUT/mocks_selftest_harness.o" \
    "$OUT/sound_mock.o" "$OUT/gpu_mock.o" "$OUT/bios_mock.o"

"$QEMU" "$OUT/mocks_selftest"

# --- the loud-failure probes ----------------------------------------------
fail=0
for probe in a b c; do
    set +e
    out="$("$QEMU" "$OUT/mocks_selftest" "$probe" 2>&1)"
    rc=$?
    set -e
    if [ "$rc" -ne 2 ]; then
        echo "FAIL: probe $probe exited $rc, expected 2 (an unsupported call must abort)"
        echo "$out"
        fail=1
    fi
done
if [ "$fail" -ne 0 ]; then
    exit 1
fi
echo "mocks-selftest: unsupported A0 service, unknown B0 service and unknown table all abort (exit 2)"
