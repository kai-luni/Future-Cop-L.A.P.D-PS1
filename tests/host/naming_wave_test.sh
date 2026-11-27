#!/bin/bash
# Compile the units renamed in the naming wave and run their behavioural checks
# under QEMU. The units are the unchanged decomp sources, so this ties the
# readable symbol names to observable behaviour.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-naming-wave}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile() {
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}

objects=()
for unit in 80035184 800336A8 80033608 80042844; do
    compile "$(unit_src "$unit")" "$OUT/$unit.o"
    objects+=("$OUT/$unit.o")
done
compile "$HERE/naming_wave_harness.c" "$OUT/naming_wave_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/naming_wave" \
    "$OUT/crt0.o" "$OUT/naming_wave_harness.o" "${objects[@]}"
"$QEMU" "$OUT/naming_wave"
