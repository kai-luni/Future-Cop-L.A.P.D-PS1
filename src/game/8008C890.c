// Categories: Platform > Timer | Input > Controller
#include "game/types.h"

extern u32 gCountWrapLimit __asm__("D_800A6E18");
extern u32 gTimeoutThreshold __asm__("D_800A9448");
void gStartTimer2Timeout(u32 a0) __asm__("func_8008C890");


/**
 * @brief Arms the PS1 Root Counter 2 timeout state for serial controller/pad polling.
 *
 * Confirmed: reads the 16-bit counter at hardware register 0x1F801120 (PS1 Root Counter 2 value), writes a0 to D_800A9448, and writes the sampled counter to D_800A6E18.
 * Category follows direct MMIO read of 0x1F801120 paired with func_8008C8B0 timeout checks across the SIO controller polling routines.
 * Likely: PSX pad library wait-timer start recording timeout threshold in sysclk/8 ticks (compared later in func_8008C8B0 and inline in func_8008AB80); Unknown: none.
 *
 * @param a0 Timeout duration threshold stored in D_800A9448 (e.g., 60, 0x50, 0x91, or 400).
 *
 * @par Globals
 * D_800A9448 - receives timeout duration a0; D_800A6E18 - receives starting 16-bit counter snapshot from 0x1F801120.
 * @par Control flow
 * Linear leaf function; reads volatile u16 at 0x1F801120 and stores both globals without branches.
 * @par Calls
 * None.
 * @par Side effects
 * Reads volatile MMIO 0x1F801120 and overwrites globals D_800A9448 and D_800A6E18.
 * @par Callers
 * Confirmed: func_8008A6CC (0x8008A718), func_8008A8E0 (0x8008A934), func_8008AB80 (0x8008ABF4, 0x8008AD30), func_8008BA34 (0x8008BB18, 0x8008BC98, 0x8008BD10) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Arms a PS1 Root Counter 2 timeout for pad polling by sampling the 16-bit
 * counter at 0x1F801120 into gCountWrapLimit (D_800A6E18) and storing the
 * duration threshold a0 into gTimeoutThreshold (D_800A9448). Called by
 * func_8008A6CC, func_8008A8E0, func_8008AB80, and func_8008BA34.
 */
void gStartTimer2Timeout(u32 a0)
{
    u32 count;

    count = *(volatile u16 *)0x1F801120;
    gTimeoutThreshold = a0;
    gCountWrapLimit = count;
}
