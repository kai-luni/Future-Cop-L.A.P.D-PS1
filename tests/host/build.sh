#!/bin/bash
# Build the freestanding MIPS host harness against the ORIGINAL decomp sources
# in THIS repo and run it under qemu-mipsel with the shared golden vectors.
# The harness is test-only: it compiles into tests/host/build/ with its own
# flags and never touches the production matching build (config/us + build/us),
# so retail-matching bytecode of the original C files is unaffected.
# Env: VECTORS (golden file; default: sibling future_cop_dotnet repo),
#      QEMU (default user-local extract), OUT (build dir).
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="$(cd "$HERE/../.." && pwd)"
source "$HERE/unit_path.sh"
T="$DECOMP/.toolchain"
GCC="$T/gcc-2.7.2-cdk/gcc"
AS="$T/mipsel-binutils/root/usr/bin/mipsel-linux-gnu-as"
LD="$T/mipsel-binutils/root/usr/bin/mipsel-linux-gnu-ld"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
QEMU="${QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}"
VECTORS="${VECTORS:-$HERE/../../../future_cop_dotnet/tests/golden/3dtl.vectors}"
OUT="${OUT:-$HERE/build}"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

compile() { # compile <src.c> <obj>
    local src="$1" obj="$2"
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/src/game" -I"$DECOMP/include" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    "$AS" -o "$obj" "$OUT/tmp.mas.s"
}

for f in 80012E1C 80012E48 8001306C 800131A0 8001996C 80063C5C 80015AF4 80015B68 80016A64; do
    compile "$(unit_src "$f")" "$OUT/$f.o"
done
compile "$HERE/gte_ref.c" "$OUT/gte_ref.o"
compile "$HERE/stubs.c" "$OUT/stubs.o"
compile "$HERE/harness.c" "$OUT/harness.o"
"$AS" -o "$OUT/crt0.o" "$HERE/crt0.s"
"$LD" -static -e __start -o "$OUT/harness" \
    "$OUT/crt0.o" "$OUT/harness.o" "$OUT/stubs.o" "$OUT/gte_ref.o" \
    "$OUT/80012E1C.o" "$OUT/80012E48.o" "$OUT/8001306C.o" "$OUT/800131A0.o" "$OUT/8001996C.o" "$OUT/80063C5C.o" \
    "$OUT/80015AF4.o" "$OUT/80015B68.o" "$OUT/80016A64.o"
echo "built $OUT/harness"
RENDER="${RENDER:-$HERE/../../../future_cop_dotnet/tests/golden/render.vectors}"
CBMP="${CBMP:-$HERE/../../../future_cop_dotnet/tests/golden/cbmp.vectors}"
"$QEMU" "$OUT/harness" "$VECTORS" "$RENDER" "$CBMP"