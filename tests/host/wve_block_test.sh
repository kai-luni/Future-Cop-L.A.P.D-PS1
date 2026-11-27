#!/bin/bash
# Link the unmodified block decode unit (table build + VLC decode) and drive it with
# the common golden vectors. MODE=emit prints the C output for regenerating them.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-$HOME/.hermes/cache/scratch}/wve-block-c.XXXXXX")}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${WVE_BLOCK_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/wve-block.vectors}"
MODE="${MODE:-check}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
compile() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$2" "$OUT/tmp.mas.s"
}
compile "$DECOMP/src/game/gpu-2d-hud/gBuildMovieVlcTables.c" "$OUT/80089A88.o"
compile "$DECOMP/src/game/gpu-2d-hud/gMovieDecodeVlcStream.c" "$OUT/80089BBC.o"
compile "$HERE/wve_block_harness.c" "$OUT/wve_block_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/wve_block_harness" \
    "$OUT/crt0.o" "$OUT/wve_block_harness.o" "$OUT/80089A88.o" "$OUT/80089BBC.o"
if [ "$MODE" = emit ]; then
    timeout 20 "$QEMU" "$OUT/wve_block_harness" "$VECTORS" emit
else
    timeout 20 "$QEMU" "$OUT/wve_block_harness" "$VECTORS"
fi
