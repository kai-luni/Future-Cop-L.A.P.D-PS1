// Categories: Graphics > Lighting | Resources > Memory Pools
#include "game/types.h"

extern void *gSpriteDescriptorTable __asm__("D_8009F77C");
extern void *gLightGrid __asm__("D_8009F780");
extern s32 gLightNodePool __asm__("D_8009F784");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void func_80019584(s32 pool);
void gReleaseMapLightData(void) __asm__("func_8004963C");


/**
 * @brief Frees and resets the map-light and effect descriptor globals D_8009F77C, D_8009F784, and D_8009F780.
 *
 * Confirmed: releases D_8009F77C and D_8009F780 via func_800181D4 when non-null, frees FixedPool handle D_8009F784 via func_80019584, and zeroes all three pointers. Counterpart to func_80049690/func_800496D0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F77C - sprite/effect descriptor table pointer freed and zeroed; D_8009F784 - 0x180-node light pool handle freed and zeroed; D_8009F780 - light bucket grid freed and zeroed.
 *
 * @par Control flow
 * Straight-line teardown with non-null guards on D_8009F77C and D_8009F780 and an unconditional free on D_8009F784.
 *
 * @par Calls
 * func_800181D4 - frees D_8009F77C and D_8009F780; func_80019584 - forwards D_8009F784 to func_800181D4.
 *
 * @par Side effects
 * Frees heap allocations and clears D_8009F77C, D_8009F784, and D_8009F780 to 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D80 in asm/us/main.s, global subsystem reset) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Frees the map light and sprite-descriptor data: the descriptor table
 * D_8009F77C (if set), the 0x180 x 0x44-byte light-node pool D_8009F784
 * (through func_80019584) and the per-cell light grid D_8009F780 (if set),
 * zeroing all three pointers. Its only caller is gResetMissionSubsystems
 * (func_80024D14), the between-mission reset; the pool and table pointers are
 * set up by func_80049690 and the grid is allocated per map cell by
 * func_800496D0 from gInitMapFromGrdb. It is the teardown counterpart of
 * level construction and touches neither D_8009F778 nor the grid size words.
 */
void gReleaseMapLightData(void)
{
    if (gSpriteDescriptorTable != 0) {
        gFreeBlock(gSpriteDescriptorTable);
    }
    gSpriteDescriptorTable = 0;
    func_80019584(gLightNodePool);
    gLightNodePool = 0;
    if (gLightGrid != 0) {
        gFreeBlock(gLightGrid);
    }
    gLightGrid = 0;
}
