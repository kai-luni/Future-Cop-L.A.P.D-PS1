// Categories: Game Engine > State Machine | Gameplay > Entity Script
#include "game/types.h"

extern s32 *gVmCountdowns __asm__("D_8009F9A4");
void gStepScriptVmCountdowns(s32 amount) __asm__("func_80069D0C");


/**
 * @brief Decrements all positive script-VM countdown entries in D_8009F9A4[0..31] by amount, clamping values below 5 to 0.
 *
 * Confirmed: loops i = 0..0x1F across the 32-word bank D_8009F9A4; for each *timer > 0, computes value = *timer - amount, clamps value < 5 to 0, and writes *timer = value. Cross-referencing func_80069B58 and func_80069D68 confirms D_8009F9A4 stores script variables scaled by 5 (read as / 5, written as * 5, reset to -10).
 * Category follows script-VM bank D_8009F9A4, per-frame invocation when unpaused, and VM interpreter func_80069D68.
 * Likely: ticks down up to 32 script-accessible countdown timers by elapsed frame delta each frame, snapping sub-1-unit remainders (< 5) to 0; Unknown: none.
 *
 * @param amount Frame delta subtracted from each positive D_8009F9A4 entry (D_8009F9FC in func_80027510).
 *
 * @par Globals
 * D_8009F9A4 - pointer to 32-element s32 script-VM timer bank (read and decremented in place).
 * @par Control flow
 * Counted 32-iteration for-loop with positive-entry guard (*timer > 0) and threshold clamp (value < 5 -> 0).
 * @par Calls
 * None.
 * @par Side effects
 * Decrements active positive timers in D_8009F9A4[0..31] in place.
 * @par Callers
 * Confirmed: func_80027510 (0x800277B4 in asm/us/main.s, when !(flags & 0x20)); func_800271A4 (0x80027354 in asm/us/main.s, with 0x7E8($gp)) / Likely: none / Unknown: none.
 */
void gStepScriptVmCountdowns(s32 amount)
{
    s32 i;
    s32 *timer;
    register s32 value __asm__("$3");

    i = 0;
    timer = gVmCountdowns;
    for (; i < 0x20; i++, timer++) {
        if (*timer > 0) {
            value = *timer - amount;
            if (value < 5) {
                value = 0;
            }
            *timer = value;
        }
    }
}
