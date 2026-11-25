#!/bin/bash
# Run the UNCHANGED retail units behind Precinct Assault's moving map objects
# (type-10 lift chain and the mounted-turret carrier chain) under qemu-mipsel.
#
# Usage: mover_objects_test.sh
# Builds into tests/host/build-movers (gitignored) and prints a final
# "movers: ALL PASS (N cases)" line, exiting non-zero on any failure.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="$(cd "$HERE/../.." && pwd)"
T="$DECOMP/.toolchain"
OUT="$DECOMP/tests/host/build-movers"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

if [ ! -x "$GCC" ] || [ ! -x "$QEMU" ]; then
    echo "host-movers: qemu or C toolchain missing, skipped"
    exit 0
fi

compile_s() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$2"
}

# The units under test: unchanged retail C, compiled with the pinned pipeline.
UNITS="80041B6C 80041488 80041880 80041728 80039504 800395A8 80039298 80039418"
OBJS=""
for unit in $UNITS; do
    compile_s "$DECOMP/src/game/$unit.c" "$OUT/$unit.mas.s"
    mipsel-linux-gnu-as -o "$OUT/$unit.o" "$OUT/$unit.mas.s"
    OBJS="$OBJS $OUT/$unit.o"
done

"$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -I"$OUT" -O1 -G0 \
    -S "$HERE/mover_objects_harness.c" -o "$OUT/tmp.s"
python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/harness.mas.s"
mipsel-linux-gnu-as -o "$OUT/harness.o" "$OUT/harness.mas.s"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
printf '.section .retailram,"aw",@nobits\n.globl D_RAM\nD_RAM:\n.space 0x9C000\n' > "$OUT/ram.s"
mipsel-linux-gnu-as -o "$OUT/ram.o" "$OUT/ram.s"

# Every retail data symbol lands at (retail address - 0x70000000).
DEFS=""
for s in $(mipsel-linux-gnu-nm -u $OBJS "$OUT/harness.o" | awk '{print $2}' | grep -E '^D_8[0-9A-F]{7}$' | sort -u); do
    DEFS="$DEFS --defsym=$s=$(printf '0x%X' $((0x${s#D_} - 0x70000000)))"
done
mipsel-linux-gnu-ld -static -e __start --section-start=.retailram=0x10010000 $DEFS \
    -o "$OUT/harness" "$OUT/crt0.o" "$OUT/harness.o" $OBJS "$OUT/ram.o"

"$QEMU" "$OUT/harness"
