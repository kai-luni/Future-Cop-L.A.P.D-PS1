#!/bin/bash
# Host tests for the map-placed object / base ownership / unit spawner path.
#
# Compiles the *unchanged* retail units below with GCC 2.7.2 + maspsx and runs
# them under qemu-mipsel against the fixtures in map_objects_harness.c:
#
#   8003FA34 gQueueOrApplyMapOccupancy   8003BC0C gSpawnEntityPrimaryType
#   8003B838 gGetSpawnDescColumn         8003BF50 gGetEntitySpawnBank
#   8003B65C gQuarterIndex               8003BA50 gFlagQuartileChange
#   800195A4 gTakeFixedPoolSlot          8002D1A0 gTryCaptureObjective
#   80060E74 gRegisterUnitSpawner        80019CA0 gFindSectionHeader
#   80019C38 gFindSectionPayload         80019BF0 gFindRegisteredResource
#   80019A74 gAppendResourceRegistryEntry 8003FAD4 gFinishLevelLoad
#   8002D664 gHandleLinkedPropEntity
#
# Memory model: qemu-mipsel cannot map guest addresses >= 0x80000000, so the
# retail RAM window lives at (retail address - 0x70000000) in a .retailram
# section relocated to 0x10010000. Every D_ symbol the units reference is
# defined by --defsym at its relocated address; the harness never reads the
# executable image.
#
# Usage: bash tests/host/map_objects_test.sh
# Exit code 0 only when every case passed.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="$(cd "$HERE/../.." && pwd)"
T="${TOOLCHAIN:-$DECOMP/.toolchain}"
OUT="${OUT:-$HERE/build-map-objects}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"

mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile_s() {
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$2"
}

# The matching msg-5 body also embeds absolute RAM operands. Relocate
# just these operands, not the instructions or any game data, for qemu.
relocate_linked_prop_ram() {
    local asm=$1
    local literals='^lw[[:blank:]]+\$[0-9]+,-214682(9812|9676|9844)$|^lui[[:blank:]]+\$[0-9]+,\(-2146828288 >> 16\) & 0xFFFF$'
    if [ "$(grep -Ec "$literals" "$asm")" != 7 ]; then
        echo "map-objects: unexpected literal RAM references in func_8002D664" >&2
        exit 2
    fi
    sed -Ei '
        s/^(lw[[:blank:]]+\$[0-9]+,)-2146829812$/\1D_8009FA0C/
        s/^(lw[[:blank:]]+\$[0-9]+,)-2146829676$/\1D_8009FA94/
        s/^(lw[[:blank:]]+\$[0-9]+,)-2146829844$/\1D_8009F9EC/
        s/^(lui[[:blank:]]+\$[0-9]+,)\(-2146828288 >> 16\) & 0xFFFF$/\10x100A/
    ' "$asm"
}

# The unit list is explicit on purpose: it is the claim this suite makes.
UNITS="8003FA34 8003BC0C 8003B838 8003BF50 8003B65C 8003BA50 800195A4 8002D1A0
       80060E74 80019CA0 80019C38 80019BF0 80019A74 8003FAD4 8002D664"
OBJS=""
for unit in $UNITS; do
    compile_s "$DECOMP/src/game/$unit.c" "$OUT/$unit.mas.s"
    if [ "$unit" = 8002D664 ]; then
        relocate_linked_prop_ram "$OUT/$unit.mas.s"
    fi
    mipsel-linux-gnu-as -o "$OUT/$unit.o" "$OUT/$unit.mas.s"
    OBJS="$OBJS $OUT/$unit.o"
done
compile_s "$HERE/map_objects_harness.c" "$OUT/harness.mas.s"
cp -f "$HERE/fc_testkit.h" "$OUT/fc_testkit.h" 2>/dev/null || true
mipsel-linux-gnu-as -o "$OUT/harness.o" "$OUT/harness.mas.s"
# Only this suite opts in to GPU stand-ins for the level-load replay tail.
compile_s "$HERE/mocks/gpu_mock.c" "$OUT/gpu_mock.mas.s"
mipsel-linux-gnu-as -o "$OUT/gpu_mock.o" "$OUT/gpu_mock.mas.s"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
printf '.section .retailram,"aw",@nobits\n.globl D_RAM\nD_RAM:\n.space 0xA0000\n' > "$OUT/ram.s"
mipsel-linux-gnu-as -o "$OUT/ram.o" "$OUT/ram.s"

# Resolve every retail data symbol the units reference to its relocated address.
DEFS=""
for s in $(mipsel-linux-gnu-nm -u $OBJS "$OUT/harness.o" | awk '{print $2}' \
           | grep -E '^D_8[0-9A-F]{7}$' | sort -u); do
    DEFS="$DEFS --defsym=$s=$(printf '0x%X' $((0x${s#D_} - 0x70000000)))"
done
mipsel-linux-gnu-ld -static -e __start --section-start=.retailram=0x10010000 $DEFS \
    -o "$OUT/map_objects" "$OUT/crt0.o" "$OUT/harness.o" $OBJS "$OUT/gpu_mock.o" "$OUT/ram.o"

if ! "$QEMU" "$OUT/map_objects" > "$OUT/run.log" 2>&1; then
    cat "$OUT/run.log"
    echo "map-objects: FAIL (harness exited non-zero)"
    exit 1
fi
cat "$OUT/run.log"

cases=$(sed -n 's/^map-objects: cases=\([0-9]*\) checks=.*/\1/p' "$OUT/run.log" | tail -1)
checks=$(sed -n 's/^map-objects: cases=[0-9]* checks=\([0-9]*\).*/\1/p' "$OUT/run.log" | tail -1)
if ! grep -q '^map-objects: all ' "$OUT/run.log"; then
    echo "map-objects: FAIL"
    exit 1
fi
echo "map-objects: ALL PASS ($cases cases, $checks checks)"
