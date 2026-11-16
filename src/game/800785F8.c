// Categories: Game Engine > Menu System | Input > Menu Navigation
#include "game/types.h"

extern s32 D_8009F890[];
extern s32 D_8009F898[];
extern s32 D_8009F8A0[];
extern s32 D_8009F8A8[];

/**
 * @brief Clears all four two-player menu directional repeat-timer arrays.
 *
 * Resets the per-port Up, Down, Left and Right hold-start tick arrays
 * (consumed by func_80082D30 in the menu input handler) to 0 for both ports.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F890/D_8009F898/D_8009F8A0/D_8009F8A8 - 2-element s32 arrays holding
 * D_8009FA2C timestamps for Up (0x100000), Down (0x400000), Left (0x800000) and Right (0x200000), zeroed for i = 0..1.
 * @par Control flow
 * Fixed 2-iteration for loop (i = 0, 1) zeroing all four arrays.
 * @par Calls
 * None.
 * @par Side effects
 * Writes 0 to eight s32 slots across D_8009F890, D_8009F898, D_8009F8A0 and D_8009F8A8.
 * @par Callers
 * Confirmed: func_80078650 (on menu push/replace); also direct call at 0x8007878C in asm/us/main.s (menu pop).
 */
void func_800785F8(void)
{
    u32 i;

    for (i = 0; i < 2; i++) {
        D_8009F890[i] = 0;
        D_8009F898[i] = 0;
        D_8009F8A0[i] = 0;
        D_8009F8A8[i] = 0;
    }
}
