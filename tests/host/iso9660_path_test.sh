#!/bin/bash
# ISO-9660 path formatting driven through the shared BIOS mock.
#
# The unit under test (src/game/memory-resources/gFormatIso9660Path.c) is the unchanged decomp source. It
# reaches the PS1 BIOS for every string operation it performs, so this test is
# only meaningful with mocks/bios_mock.c behind those vectors - and it exercises
# that mock the way a test should: it asserts the real call chain, the resulting
# path, and the refusal paths.
#
# Two runs:
#   1. the behaviour checks, which must pass;
#   2. a probe that asks the mock for an unimplemented A0 service and must abort
#      with exit code 2 and the vector printed. That is the contract a silent
#      `return 0` stub would break.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-iso9660-path}"
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

compile "$DECOMP/src/game/memory-resources/gFormatIso9660Path.c" "$OUT/8001DC48.o"
compile "$HERE/mocks/bios_mock.c" "$OUT/bios_mock.o"
compile "$HERE/iso9660_path_harness.c" "$OUT/iso9660_path_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"

# The only symbols the unit may take from outside itself are the BIOS vectors;
# anything else means the mock set is incomplete and the test would be lying
# about what it covers.
unresolved="$(mipsel-linux-gnu-nm -u "$OUT/8001DC48.o" | awk '{print $NF}' | sort -u)"
expected="func_8008CC90
func_8008CCB0
func_8008CCC0
func_8008CCE0"
if [ "$unresolved" != "$expected" ]; then
    echo "FAIL: 8001DC48 needs symbols the BIOS mock does not cover:"
    echo "$unresolved"
    exit 1
fi

mipsel-linux-gnu-ld -static -e __start -o "$OUT/iso9660_path" \
    "$OUT/crt0.o" "$OUT/iso9660_path_harness.o" "${OUT}/8001DC48.o" "$OUT/bios_mock.o"

"$QEMU" "$OUT/iso9660_path"

# --- the loud-failure probe ------------------------------------------------
set +e
probe_out="$("$QEMU" "$OUT/iso9660_path" abort 2>&1)"
probe_rc=$?
set -e

if [ "$probe_rc" -ne 2 ]; then
    echo "FAIL: an unimplemented BIOS service exited $probe_rc, expected 2"
    echo "$probe_out"
    exit 1
fi
case "$probe_out" in
    *"unsupported service 153"*) ;;
    *)
        echo "FAIL: the unimplemented BIOS service was not reported by number"
        echo "$probe_out"
        exit 1
        ;;
esac
echo "iso9660-path: unmocked BIOS service aborts loudly (exit 2)"
