#!/bin/bash
# Compile the ORIGINAL projectile spawner func_8005E47C and the integrator func_8001ACA4 and run the
# shared shot vectors under QEMU. func_8004D444 is replaced by a recorder so the spawn block the
# fire path hands to the queue can be compared word by word with the .NET port.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-shot}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
SPAWN="${SHOT_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/shot.vectors}"
MOVE="${SHOT_MOVE_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/shot-move.vectors}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
compile() { # compile <src.c> <obj>
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}
compile "$DECOMP/src/game/gameplay-ai/gSpawnDirectShot.c" "$OUT/8005E47C.o"
compile "$DECOMP/src/game/utilities/gVectorAddScaled.c" "$OUT/8001ACA4.o"
compile "$HERE/shot_harness.c" "$OUT/shot_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/shot_harness" \
    "$OUT/crt0.o" "$OUT/shot_harness.o" "$OUT/8005E47C.o" "$OUT/8001ACA4.o"
"$QEMU" "$OUT/shot_harness" "$SPAWN" "$MOVE"
