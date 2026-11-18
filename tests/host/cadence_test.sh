#!/bin/bash
# Compile the ORIGINAL weapon trigger logic and run the shared cadence vectors under QEMU.
#   8006C3E0.c - reload timer decrement (per-frame, delta ticks)
#   8006BA50.c - slot update: fires when the timer is zero, then reloads it from the entry byte
# func_8006AAEC (registry) and func_8006ACB0 (fire dispatcher) are stubs, so the harness records the
# frames in which retail would have fired and compares them with tests/golden/cadence.vectors.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-cadence}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${CADENCE_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/cadence.vectors}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
compile() { # compile <src.c> <obj>
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}
compile "$DECOMP/src/game/8006C3E0.c" "$OUT/8006C3E0.o"
compile "$DECOMP/src/game/8006BA50.c" "$OUT/8006BA50.o"
compile "$HERE/cadence_harness.c" "$OUT/cadence_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/cadence_harness" \
    "$OUT/crt0.o" "$OUT/cadence_harness.o" "$OUT/8006C3E0.o" "$OUT/8006BA50.o"
"$QEMU" "$OUT/cadence_harness" "$VECTORS"