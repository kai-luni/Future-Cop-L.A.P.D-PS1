#!/bin/bash
# Compile the original naming-round C units and run behavioral checks under QEMU.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-naming-round}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
compile() {
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}
objects=()
for unit in 8003C4C4 8003CF20 8002A23C 80085900 800869D8 80091544 8004D664 80045884 8005CEA8 8004953C 80029BE4 80044FF8 80047690 80049410 8002A598 8003FD6C 8002A5AC; do
    compile "$(unit_src "$unit")" "$OUT/$unit.o"
    objects+=("$OUT/$unit.o")
done
compile "$HERE/naming_round_harness.c" "$OUT/harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/harness" "$OUT/crt0.o" "$OUT/harness.o" "${objects[@]}"
"$QEMU" "$OUT/harness"
