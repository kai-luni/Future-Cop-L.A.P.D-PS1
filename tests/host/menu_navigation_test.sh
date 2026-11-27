#!/bin/bash
# Run the unchanged menu selection function against the shared vectors.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-menu-navigation}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${MENU_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/menu-navigation.vectors}"
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
compile "$DECOMP/src/game/boot-frontend/gSelectEnabledMenuItem.c" "$OUT/menu.o"
compile "$HERE/menu_navigation_harness.c" "$OUT/harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/harness" "$OUT/crt0.o" "$OUT/harness.o" "$OUT/menu.o"
"$QEMU" "$OUT/harness" "$VECTORS"
