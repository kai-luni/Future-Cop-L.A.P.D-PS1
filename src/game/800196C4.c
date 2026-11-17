// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 gFreeRangeCursor __asm__("D_8009F5AC");
extern s32 gStateEntryTable[] __asm__("D_800A04D0");

/**
 * @brief Lagged-additive random step over a 20-entry state table.
 *
 * @return The updated table entry D_800A04D0[i] (old value plus the entry
 *         three positions ahead).
 *
 * @par Confirmed
 * i = D_8009F5AC - 1 wrapped to 0x13 when negative; j = D_8009F5AC + 2
 * wrapped by -0x12 when >= 0x14; the cursor D_8009F5AC becomes i and the
 * returned entry is D_800A04D0[i] += D_800A04D0[j]. The two taps are 3 apart
 * modulo 20.
 *
 * @par Likely role
 * The gameplay random source (sibling of func_80019668): the spawn and
 * effect handlers func_8003D0F0/func_80040874 draw from it for jitter and
 * spread decisions.
 *
 * @par Callers
 * func_8003D0F0, func_80040874.
 */
s32 func_800196C4(void)
{
    s32 i;
    s32 j;

    i = gFreeRangeCursor - 1;
    j = gFreeRangeCursor + 2;
    if (i < 0) {
        i = 0x13;
    } else if (j >= 0x14) {
        j = gFreeRangeCursor - 0x12;
    }
    gFreeRangeCursor = i;
    return gStateEntryTable[i] += gStateEntryTable[j];
}
