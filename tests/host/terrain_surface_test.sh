#!/bin/bash
# Run the terrain surface units (func_80067480, func_80067684) unchanged.
#
# Both are hardware-free real C, so there is no stand-in for game logic here:
# the two data tables the sampler reads are generated straight from the retail
# executable, so the test sees the bytes the game sees.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-terrain-surface}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
ROM="${ROM:-$DECOMP/baseroms/us/SLUS_007.39}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

# D_8009AFE8 (64 x 4 corner bytes) and D_8009B1A4 (64 orientation bytes). The
# image is loaded at 0x80010000 with an 0x800-byte header, so a virtual address
# maps to 0x800 + (addr - 0x80010000). The generator refuses to write a table
# that does not read back identical.
python3 - "$ROM" "$OUT/terrain_tables.h" <<'PY'
import sys
from pathlib import Path
rom = Path(sys.argv[1]).read_bytes()
BASE, HDR = 0x80010000, 0x800
def read(addr, n):
    off = HDR + (addr - BASE)
    return rom[off:off + n]
corners = [tuple(read(0x8009AFE8 + i * 4, 4)) for i in range(64)]
shapes = list(read(0x8009B1A4, 64))
out = []
out.append("/* Generated from SLUS_007.39 by terrain_surface_test.sh - do not edit. */\n")
out.append("u8 gTileCornerIndices[64][4] __asm__(\"D_8009AFE8\") = {\n")
for c in corners:
    out.append("{" + ",".join(str(b) for b in c) + "},\n")
out.append("};\n\n")
out.append("u8 gTileShapeTable[64] __asm__(\"D_8009B1A4\") = {\n")
out.append(",".join(str(b) for b in shapes) + ",\n};\n")
text = "".join(out)
# Round-trip the corner table before writing: a transposed or byte-swapped dump
# would look plausible and be wrong.
for i, c in enumerate(corners):
    got = tuple(int(v) for v in out[2 + i].strip().strip("{}, ").split(","))
    assert got == c, f"corner {i} round-trip {got} != {c}"
Path(sys.argv[2]).write_text(text)
print(f"terrain tables: {len(corners)} corner rows, {len(shapes)} shape bytes")
PY

compile() {
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -I"$HERE" -I"$OUT" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}

compile "$DECOMP/src/game/80067480.c" "$OUT/80067480.o"
compile "$DECOMP/src/game/80067684.c" "$OUT/80067684.o"
compile "$HERE/terrain_surface_harness.c" "$OUT/terrain_surface_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/terrain_surface" \
    "$OUT/crt0.o" "$OUT/terrain_surface_harness.o" \
    "$OUT/80067480.o" "$OUT/80067684.o"
"$QEMU" "$OUT/terrain_surface"
