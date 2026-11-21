// Categories: Graphics > Terrain | Game Engine > Init
#include "game/types.h"

extern void *D_800A294C[];
extern u8 gSlfxFlagByte[] __asm__("D_800A2931");
extern s32 gQueueCount __asm__("D_8009F838");
void gClearTerrainState(void) __asm__("func_80063134");


/**
 * @brief Clears terrain heightfield pointer, ground-object table count, and SLFX flag byte.
 *
 * Resets D_800A294C[0] (slot +0xC of heightfield descriptor D_800A2940 built by func_800695A8 and freed by func_800634F4), D_8009F838 (active count in the 24-entry D_800A31A0 collision table), and D_800A2931[0] (set by func_800639D4 on SLFX chunks).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A294C - terrain heightfield base pointer (D_800A2940[3]), cleared to 0.
 * D_8009F838 - entry count for D_800A31A0 ground-collision table (func_8006320C, func_8006314C), cleared to 0.
 * D_800A2931 - SLFX flag byte written during map chunk parsing in func_800639D4, cleared to 0.
 *
 * @par Control flow
 * Straight-line stores of zero to all three globals, then return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 0 to D_800A294C[0], D_8009F838, and D_800A2931[0].
 *
 * @par Callers
 * Confirmed: func_80063428 (0x80063450, terrain/map init during func_80024A24 mission boot) / Likely: none / Unknown: none.
 */
void gClearTerrainState(void)
{
    D_800A294C[0] = 0;
    gQueueCount = 0;
    gSlfxFlagByte[0] = 0;
}
