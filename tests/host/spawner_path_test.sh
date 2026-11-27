#!/bin/bash
# Run the unchanged unit-spawner units of the Precinct Assault order path.
# Usage: spawner_path_test.sh [--emit]
#   default  checks the vectors of spawner_path.vectors
#   --emit   rewrites spawner_path.vectors with the observed values
# OUT defaults to a temporary directory, so nothing is written into the repo
# unless --emit is passed.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="$(cd "$HERE/../.." && pwd)"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-/tmp}/spawner-path.XXXXXX")}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
VECTORS="$HERE/spawner_path.vectors"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile_s() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$2"
}

UNITS="800616A0 8006125C 8006158C 800614BC"
OBJS=""
for unit in 800616A0 8006125C 8006158C 800614BC; do
    compile_s "$(unit_src "$unit")" "$OUT/$unit.mas.s"
    mipsel-linux-gnu-as -o "$OUT/$unit.o" "$OUT/$unit.mas.s"
    OBJS="$OBJS $OUT/$unit.o"
done
python3 "$HERE/spawner_path_vectors.py" "$VECTORS" > "$OUT/spawner_path_vectors.h"
"$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -I"$OUT" -O1 -G0 \
    -DSPAWNER_VECTORS='gSpawnerVectors' -S "$HERE/spawner_path_harness.c" -o "$OUT/tmp.s"
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

if [ "${1:-}" = "--emit" ]; then
    "$QEMU" "$OUT/harness" "$VECTORS" --emit > "$VECTORS.new"
    mv "$VECTORS.new" "$VECTORS"
    echo "spawner-path: vectors rewritten ($VECTORS)"
else
    "$QEMU" "$OUT/harness" "$VECTORS"
fi
