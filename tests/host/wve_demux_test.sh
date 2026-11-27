#!/bin/bash
# Link the unmodified demux unit and drive the common .NET golden vectors.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-$HOME/.hermes/cache/scratch}/wve-demux-c.XXXXXX")}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${WVE_DEMUX_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/wve-demux.vectors}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
compile() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$2" "$OUT/tmp.mas.s"
}
compile "$DECOMP/src/game/video-mdec-movies/gMovieStreamDemux.c" "$OUT/800879DC.o"
compile "$HERE/wve_demux_harness.c" "$OUT/wve_demux_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/wve_demux_harness" "$OUT/crt0.o" "$OUT/wve_demux_harness.o" "$OUT/800879DC.o"
timeout 15 "$QEMU" "$OUT/wve_demux_harness" "$VECTORS"
