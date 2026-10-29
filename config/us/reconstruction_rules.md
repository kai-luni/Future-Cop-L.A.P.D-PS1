# Reconstruction rules for this target

Ground rules learned from the 522 already-verified units and from hand checks
against SLUS-00739. Follow them before guessing.

## Small data and `$gp`

1. If the disassembly oracle contains `($gp)`, the referenced symbol lives in
   `.sbss`/`.sdata` and must be declared as a plain typed `extern`
   (`extern u8 *D_8009E2A8;`, `extern s16 D_8009F580;`). Never synthesise the
   absolute address of such a symbol with casts or integer literals.
2. Compile with a small-data threshold that is not `-G0`: `-G4` or `-G8` makes
   `extern` objects reachable through `$gp`. With `-G0` every access is emitted
   as `lui`/`lw` and can never match a `($gp)` oracle.
3. A gp access that differs only in its offset (`0x0($gp)` where the oracle has
   `0x2A8($gp)`) already has the correct code shape. Do not restructure the C:
   report the expected offset in `note` and place the object through
   `small_data_symbols` instead.
   The verified units show how that is declared: an `extern` object listed in
   `small_data_symbols` (name -> size, e.g. `"D_8009E2A8": 4`) plus its address in
   `symbols`, compiled with `-G4`. The object name is derived from the oracle offset:
   target address minus the `$gp` base `0x8009F214` gives the byte offset, and the
   symbol is named after the resulting address.

## BIOS dispatch stubs

4. A 12-byte stub of the form `addiu $t2,$zero,0xB0` / `jr $t2` /
   `addiu $t1,$zero,0xNN` is a BIOS A0/B0 table call, with the function number in
   `$t1` and the dispatcher address in `$t2`. This cannot be expressed in plain C:
   emit a `void` function named after the unit whose body is one
   `__asm__ volatile("...")` block holding the bare instructions, with the clobbered
   registers listed. Inline assembly is accepted for these dispatchers only, and only
   as instructions: labels, `.globl`/`.type`/`.ent`/`.set`/`.text` directives and
   `.word` encodings are rejected, because that would be an assembler file
   reproducing the retail bytes instead of reconstructed C.
   Note: even a correct asm block still gets the compiler's own prologue/epilogue
   appended, so a 12-byte `$t2` stub can never reach the exact extent. 45 such
   ranges (1,600 bytes) exist in the executable; the worker skips them.

## Style that matches this project

5. `similar_verified_units` in the request holds already-verified sources with the
   same instruction mix. Mirror their declaration and control-flow style first; they
   replicate this compiler's quirks better than any general rule.
6. Include shared headers as `#include "game/types.h"`, use the project's
   `s8/u8/s16/u16/s32/u32` types, and keep functions `void`-arg lists explicit.
6. Use the compiler the unit actually needs (2.6.3, 2.7.2-cdk, 2.7.2-psx,
   2.8.0-psx, 2.8.1-psx). Scheduling and epilogue placement differ, so try the
   pinned alternatives before assuming the C is wrong.
7. Prefer the shape of an already-verified unit in `src/game/` over a clever
   rewrite; the byte verifier rewards the original compiler's quirks.
8. When a mismatch cannot be explained by the rules above, say so in `note` and
   return the closest honest candidate instead of inventing structure.
9. `matching_prefix_bytes` equal to the full size with extra trailing bytes means
   the instructions are right but the function still emits an epilogue. Do not
   hide that with inline assembly; it is a compiler-boundary case, not a C bug.
