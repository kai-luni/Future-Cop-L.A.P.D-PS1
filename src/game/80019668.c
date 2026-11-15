// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 D_8009F5A8;
extern s32 D_800A0480[];

/* additive Lagged-Fibonacci-Generator, 31 bit random number */
/**
 * @brief Random number generator: additive lagged Fibonacci, 31 bit.
 *
 * @return A pseudo-random 31-bit value.
 *
 * @par Confirmed
 * i = D_8009F5A8 - 1 and j = D_8009F5A8 + 2, both wrapped into 0..0x13 (20
 * entries); the cursor becomes i, and the return value is
 * D_800A0480[i] += D_800A0480[j] - an additive lagged Fibonacci step with lags
 * 1 and 2 over a 20-word ring, which is also what the retail comment above the
 * function says ("additive Lagged-Fibonacci-Generator, 31 bit random number").
 *
 * @par Callers
 * 57 sites across the whole game (gameplay, effects, particles), which is why
 * the same generator is also the source of the "random" behaviour in the
 * renderers.
 *
 * @par Note
 * The state table D_800A0480 is filled elsewhere (func_800197A8 seeds it); this
 * function only reads and advances it.
 */
s32 func_80019668(void)
{
    s32 i;
    s32 j;

    i = D_8009F5A8 - 1;
    j = D_8009F5A8 + 2;
    if (i < 0) {
        i = 0x13;
    } else if (j >= 0x14) {
        j = D_8009F5A8 - 0x12;
    }
    D_8009F5A8 = i;
    return D_800A0480[i] += D_800A0480[j];
}
