#!/bin/bash
# Compile the original environment/matrix render cores (C units and the
# handwritten GTE asm units, all unmodified from src/game) and run the
# hand-derived vectors in tests/host/vectors/env.vectors under QEMU.
# cop2 instructions trap to the software GTE in env_harness.c (gte_ref.c model).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-env}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${ENV_VECTORS:-$HERE/vectors/env.vectors}"
ROM="${ROM:-$DECOMP/baseroms/us/SLUS_007.39}"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
# Retail quarter-wave sine table D_8009A234 (1025 u16) straight from the executable.
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
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}
UNITS="8001846C 800188D0 80018DC0 80019470 8001A81C 80018860 80013CB8 80010750 80010820 8001006C 8001010C 800101D4 80010234 8001047C"
OBJS=""
for f in $UNITS; do
    compile "$(unit_src "$f")" "$OUT/$f.o"
    OBJS="$OBJS $OUT/$f.o"
done
compile "$OUT/sine.c" "$OUT/sine.o"
compile "$HERE/gte_ref.c" "$OUT/gte_ref.o"
compile "$HERE/env_harness.c" "$OUT/env_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s" 2>/dev/null
mipsel-linux-gnu-ld -static -e __start -o "$OUT/env_harness" \
    "$OUT/crt0.o" "$OUT/env_harness.o" "$OUT/gte_ref.o" "$OUT/sine.o" $OBJS
"$QEMU" "$OUT/env_harness" "$VECTORS"
