// Categories: Game Engine > Init | Resources > Spawn Tables | Resources > Memory Pools
#include "game/types.h"

struct FixedPool;

extern s32 D_800A0C70[];
extern s32 D_800A7040[2][256];
extern s32 D_800A1070[];

extern s32 D_8009F310;
extern s32 D_8009F314;
extern struct FixedPool *D_8009F70C;

extern void func_8006EDC4(void);
extern void func_8005E1BC(void);
extern struct FixedPool *func_800194DC(s32 count, s32 size, s32 tag);

/**
 * @brief Resets spawn and occupancy tables and initializes mission-level object pools.
 *
 * Clears the 256-entry snapshot array D_800A0C70, both 256-entry rows of D_800A7040,
 * D_800A1070[0..6], and state counters, initializes helper pools via func_8006EDC4 and
 * func_8005E1BC, and allocates the 256-node 8-byte deferred-link pool D_8009F70C.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0C70, D_800A7040, D_800A1070 - zeroed tables; D_8009F310, D_8009F314 - cleared indices; D_8009F70C - receives new FixedPool.
 *
 * @par Control flow
 * Reverse-counts 256 entries in D_800A0C70 and 2x256 entries in D_800A7040, initializes sub-pools, allocates D_8009F70C, and zeroes 7 words of D_800A1070.
 *
 * @par Calls
 * func_8006EDC4 - initializes pool D_800A3538 (0x48 x 10); func_8005E1BC - initializes pools D_800A22A0/B0; func_800194DC - allocates 0x100 x 8-byte tag-1 pool.
 *
 * @par Side effects
 * Clears D_800A0C70, D_800A7040, D_800A1070, D_8009F310, D_8009F314, allocates heap memory for D_8009F70C, and resets sub-pools.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CEC, mission boot sequence).
 */
void func_8003F79C(void)
{
    s32 i;
    s32 j;

    for (i = 0xFF; i >= 0; i--) {
        D_800A0C70[i] = 0;
    }

    for (j = 0; j < 2; j++) {
        s32 *p = D_800A7040[j];
        for (i = 0xFF; i >= 0; i--) {
            p[i] = 0;
        }
    }

    D_8009F310 = 0;
    func_8006EDC4();
    func_8005E1BC();
    D_8009F70C = func_800194DC(0x100, 8, 1);
    D_8009F314 = 0;

    for (i = 6; i >= 0; i--) {
        D_800A1070[i] = 0;
    }
}
