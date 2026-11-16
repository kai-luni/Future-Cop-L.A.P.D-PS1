// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 D_8009F214;
extern s32 D_8009F5A8;
extern s32 D_8009F5AC;
extern s32 D_800A0480[];
extern s32 D_800A04D0[];

extern s32 func_80019720(void);

/**
 * @brief Seeds the PRNG LCG state and populates both 20-entry lagged-Fibonacci ring buffers.
 *
 * Stores seed into D_8009F214, resets both ring indices D_8009F5A8 and D_8009F5AC to 0x14 (20), and fills the visual/effect table D_800A0480 and gameplay table D_800A04D0 with 40 consecutive outputs of func_80019720.
 *
 * @param seed Initial 32-bit LCG seed written to D_8009F214.
 *
 * @par Globals
 * D_8009F214 (LCG seed), D_8009F5A8 and D_8009F5AC (ring cursors reset to 0x14), D_800A0480 and D_800A04D0 (20-word PRNG ring tables).
 *
 * @par Control flow
 * Writes seed and both cursors, then runs two sequential 20-iteration loops calling func_80019720 to fill D_800A0480 and D_800A04D0.
 *
 * @par Calls
 * func_80019720 - Park-Miller 31-bit LCG (multiplier 16807) advancing D_8009F214 per entry.
 *
 * @par Side effects
 * Rewrites D_8009F214, D_8009F5A8, D_8009F5AC, D_800A0480[0..19], and D_800A04D0[0..19].
 *
 * @par Callers
 * Confirmed: func_80019828 (0x800198B4, boot seed 0x457), func_80024904 (0x8002496C), func_80024A24 (0x80024BC0, 0x80024BF0), and func_80059BFC (0x80059C1C).
 */
void func_800197A8(s32 seed)
{
    s32 i;

    D_8009F214 = seed;
    D_8009F5A8 = 0x14;
    D_8009F5AC = 0x14;
    for (i = 0; i < 0x14; i++) {
        D_800A0480[i] = func_80019720();
    }
    for (i = 0; i < 0x14; i++) {
        D_800A04D0[i] = func_80019720();
    }
}
