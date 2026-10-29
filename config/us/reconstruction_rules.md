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

## BIOS dispatch stubs

4. A 12-byte stub of the form `addiu $t2,$zero,0xB0` / `jr $t2` /
   `addiu $t1,$zero,0xNN` is a BIOS A0/B0 table call, with the function number in
   `$t1` and the dispatcher address in `$t2`. This cannot be expressed in plain C:
   emit a `void` function whose body is one `__asm__ volatile(...)` block with the
   three instructions, naming the clobbered registers. Inline assembly is accepted
   for these dispatchers (and only for them).

## Style that matches this project

5. Include shared headers as `#include "game/types.h"`, use the project's
   `s8/u8/s16/u16/s32/u32` types, and keep functions `void`-arg lists explicit.
6. Use the compiler the unit actually needs (2.6.3, 2.7.2-cdk, 2.7.2-psx,
   2.8.0-psx, 2.8.1-psx). Scheduling and epilogue placement differ, so try the
   pinned alternatives before assuming the C is wrong.
7. Prefer the shape of an already-verified unit in `src/game/` over a clever
   rewrite; the byte verifier rewards the original compiler's quirks.
8. When a mismatch cannot be explained by the rules above, say so in `note` and
   return the closest honest candidate instead of inventing structure.
