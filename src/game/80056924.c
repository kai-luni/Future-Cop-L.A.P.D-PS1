// Categories: Gameplay > Camera > Preset Sequence | Utilities > Random Numbers
#include "game/types.h"

extern u8 gSideStepTable[][8] __asm__("D_800A2290");

extern u32 func_80019668(void);

/**
 * @brief Fills the two per-side camera preset sequences with random entries.
 *
 * D_800A2290 is indexed [side][step] by func_80056994, which the chase camera
 * func_800569EC uses to replace its preset byte while D_8009FA84 is positive.
 * Each of the sixteen bytes becomes (random & 7) + 7, so every entry lands in
 * 7..14, and every byte is drawn separately from the game's lagged-Fibonacci
 * generator. Mission boot seeds that generator immediately before this call.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_800A2290 - the two-row, 8-byte-per-row table. This function is its only
 * writer in the decompiled set.
 *
 * @par Control flow
 * One counted loop over 0..7 with two generator draws per iteration and one store
 * at +0 (row 0) and +8 (row 1). No branches inside the body.
 *
 * @par Calls
 * func_80019668 - the 31-bit additive lagged-Fibonacci generator; two calls per
 * iteration, so sixteen draws for the sixteen bytes.
 *
 * @par Side effects
 * Overwrites all sixteen bytes of D_800A2290 and advances the generator state.
 *
 * @par Callers
 * Confirmed: func_80024A24 (mission boot, after the RNG seeding).
 *
 * @par Note
 * The values 7..14 lie outside the 0..7 range the camera's preset byte wraps
 * over, so either the consumer reinterprets the byte or the table is not a plain
 * preset index; the call sites only show it being read as D_800A2290[side][step].
 */
void func_80056924(void)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        gSideStepTable[0][i] = (func_80019668() & 7) + 7;
        gSideStepTable[1][i] = (func_80019668() & 7) + 7;
    }
}
