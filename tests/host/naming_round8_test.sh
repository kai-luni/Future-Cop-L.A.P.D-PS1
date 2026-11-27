#!/bin/bash
# Compile the camera phase units exercised in naming round 8 and run their behavioural checks
# under QEMU. The units are the unchanged decomp sources, so this ties the
# readable symbol names to observable behaviour.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-naming-round8}"
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
for unit in 80056924 80056994 8005B088; do
    compile "$(unit_src "$unit")" "$OUT/$unit.o"
    objects+=("$OUT/$unit.o")
done
compile "$HERE/naming_round8_harness.c" "$OUT/naming_round8_harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/naming_round8" \
    "$OUT/crt0.o" "$OUT/naming_round8_harness.o" "${objects[@]}"
"$QEMU" "$OUT/naming_round8"
