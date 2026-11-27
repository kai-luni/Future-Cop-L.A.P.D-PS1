#!/bin/bash
# Build and run the Precinct Assault path harness against the unchanged retail units.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${DECOMP:-$(cd "$HERE/../.." && pwd)}"
T="$DECOMP/.toolchain"
OUT="${OUT:-$HERE/build-precinct-path}"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"

mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
compile() {
    "$T/gcc-2.7.2-cdk/gcc" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -O1 -G0 -S "$1" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$2" "$OUT/tmp.mas.s"
}

compile "$DECOMP/src/game/boot-frontend/gSetupCrimeWarMissionList.c" "$OUT/setup_cw.o"
compile "$DECOMP/src/game/boot-frontend/gSetupPrecinctAssaultMissionList.c" "$OUT/setup_pa.o"
compile "$DECOMP/src/game/boot-frontend/gUpdatePrecinctUnlockFlag.c" "$OUT/unlock.o"
compile "$HERE/precinct_path_harness.c" "$OUT/harness.o"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s"
mipsel-linux-gnu-ld -static -e __start -o "$OUT/harness" "$OUT/crt0.o" "$OUT/harness.o" "$OUT/setup_cw.o" "$OUT/setup_pa.o" "$OUT/unlock.o"
"$QEMU" "$OUT/harness"
