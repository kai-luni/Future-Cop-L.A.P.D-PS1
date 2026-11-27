#!/bin/bash
# Compile the original joint routine and check shared vectors under QEMU.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-joints}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${JOINT_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/joints.vectors}"
ROM="${ROM:-$DECOMP/baseroms/us/SLUS_007.39}"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
python3 - "$ROM" "$OUT/sine.c" <<'PY'
import struct, sys
from pathlib import Path
rom = Path(sys.argv[1]).read_bytes()
table = struct.unpack_from('<1025H', rom, 0x8aa34)
Path(sys.argv[2]).write_text('unsigned short D_8009A234[1025] = {' + ','.join(map(str, table)) + '};\n')
PY
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
compile() {
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$DECOMP/tests/host/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}
compile "$DECOMP/src/game/utilities/gSinCos.c" "$OUT/8001846C.o"
compile "$DECOMP/src/game/utilities/gBuildRotationMatrixVariant.c" "$OUT/80018DC0.o"
compile "$OUT/sine.c" "$OUT/sine.o"
compile "$DECOMP/src/game/renderer-scene/gComposeJointChain.c" "$OUT/80052960.o"
compile "$HERE/joints_probe.c" "$OUT/joints_probe.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$DECOMP/tests/host/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/joints_harness" "$OUT/crt0.o" "$OUT/joints_probe.o" "$OUT/sine.o" "$OUT/8001846C.o" "$OUT/80018DC0.o" "$OUT/80052960.o"
"$QEMU" "$OUT/joints_harness" "$VECTORS"
