#!/bin/bash
# Compile the ORIGINAL animation stepper func_80044B34 and run the shared mech-form vectors under
# QEMU. The cases are the type-3 scrub retail uses for the walker/hover transformation.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-form}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${FORM_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/mech-form.vectors}"
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
compile "$DECOMP/src/game/gpu-2d-hud/gAdvanceAnimTrack.c" "$OUT/80044B34.o"
compile "$HERE/form_harness.c" "$OUT/form_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/form_harness" \
    "$OUT/crt0.o" "$OUT/form_harness.o" "$OUT/80044B34.o"
"$QEMU" "$OUT/form_harness" "$VECTORS"
