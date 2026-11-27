#!/bin/bash
# Run the unchanged single-player Precinct Assault front-end units against the
# vectors shared with the .NET port. Usage:
#   precinct_assault_test.sh            check tests/golden/precinct-assault.vectors
#   precinct_assault_test.sh --emit     print the vectors with observed outputs
# OUT defaults to a temporary directory, so nothing is written into the repo.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
OUT="${OUT:-$(mktemp -d "${TMPDIR:-/tmp}/precinct-assault.XXXXXX")}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${PRECINCT_VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/precinct-assault.vectors}"
SLUS="${SLUS:-$DECOMP/baseroms/us/SLUS_007.39}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile_s() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$2"
}

UNITS="800776B8 80077708 80077FFC 8007A4CC 8007A56C 8007A5F0 8007B004 8007B854 8007D39C 8007D408 8007D2DC 8007E3EC"
OBJS=""
for u in $UNITS; do
    compile_s "$(unit_src "$u")" "$OUT/$u.mas.s"
    if [ "$u" = 8007A4CC ]; then
        # func_8007A4CC stores D_800A368B through a literal `lui 0x800A`. Guest
        # addresses >= 0x80000000 are unmappable under qemu-mipsel, so relocate
        # that one constant by the harness offset (-0x70000000) like every
        # linked D_ symbol. Exactly one occurrence is required.
        n=$(grep -c 'lui	\$3,(-2146828288 >> 16) & 0xFFFF' "$OUT/$u.mas.s" || true)
        [ "$n" = 1 ] || { echo "unexpected 0x800A literal count in $u: $n" >&2; exit 2; }
        sed -i 's/lui	\$3,(-2146828288 >> 16) & 0xFFFF/lui	$3,0x100A/' "$OUT/$u.mas.s"
    fi
    mipsel-linux-gnu-as -o "$OUT/$u.o" "$OUT/$u.mas.s"
    OBJS="$OBJS $OUT/$u.o"
done
compile_s "$HERE/precinct_assault_harness.c" "$OUT/harness.mas.s"
mipsel-linux-gnu-as -o "$OUT/harness.o" "$OUT/harness.mas.s"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
printf '.section .retailram,"aw",@nobits\n.globl D_RAM\nD_RAM:\n.space 0x94000\n' > "$OUT/ram.s"
mipsel-linux-gnu-as -o "$OUT/ram.o" "$OUT/ram.s"

# Every retail data symbol lands at (retail address - 0x70000000).
DEFS=""
for s in $(mipsel-linux-gnu-nm -u $OBJS "$OUT/harness.o" | awk '{print $2}' | grep -E '^D_8[0-9A-F]{7}$' | sort -u); do
    DEFS="$DEFS --defsym=$s=$(printf '0x%X' $((0x${s#D_} - 0x70000000)))"
done
DEFS="$DEFS --defsym=D_8009F8E0_arr=0x1009F8E0"
mipsel-linux-gnu-ld -static -e __start --section-start=.retailram=0x10010000 $DEFS \
    -o "$OUT/harness" "$OUT/crt0.o" "$OUT/harness.o" $OBJS "$OUT/ram.o"
"$QEMU" "$OUT/harness" "$VECTORS" "$SLUS" "$@"
