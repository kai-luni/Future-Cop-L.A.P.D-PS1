// Categories: Platform > Timer | Input > Controller
#include "game/types.h"

extern volatile u32 D_800A6E18;
extern volatile u32 gTimeoutThreshold __asm__("D_800A9448");

/**
 * @brief Checks whether the PS1 Root Counter 2 pad-polling timeout armed by func_8008C890 has elapsed.
 *
 * Confirmed: samples 16-bit counter 0x1F801120; if count < D_800A6E18, wraps via 0x1F801128 (when nonzero) or 0x10000; then tests (0x1F801124 & 0x200) to compare (count - D_800A6E18) or ((count - D_800A6E18) >> 3) against D_800A9448.
 * Category follows direct MMIO reads of PS1 Root Counter 2 (0x1F801120 count, 0x1F801124 mode, 0x1F801128 target) and SIO controller callers.
 * Likely: PSX pad library timeout predicate normalizing Timer 2 to sysclk/8 ticks (bit 9 of mode = 0x200 selects sysclk/8 prescaler); Unknown: none.
 *
 * @param void No parameters.
 * @return Nonzero (1) if elapsed time >= D_800A9448, else 0.
 *
 * @par Globals
 * D_800A6E18 - start tick captured by func_8008C890; D_800A9448 - target timeout threshold.
 * @par Control flow
 * Wrap-adjustment check on count < D_800A6E18 (choosing 0x1F801128 or 0x10000), followed by a 0x1F801124 & 0x200 branch selecting unshifted vs >>3 delta comparison.
 * @par Calls
 * None.
 * @par Side effects
 * Reads volatile Root Counter 2 MMIO registers (0x1F801120, 0x1F801124, 0x1F801128).
 * @par Callers
 * Confirmed: func_8008A9A8 (0x8008AA24, 0x8008AAF8), func_8008AB80 (0x8008AD38), func_8008ADB4 (0x8008ADE8) / Likely: none / Unknown: none.
 */
s32 func_8008C8B0(void)
{
    u32 count;

    count = *(volatile u16 *)0x1F801120 & 0xFFFF;

    if (count < D_800A6E18) {
        if (*(volatile u16 *)0x1F801128 != 0) {
            count += *(volatile u16 *)0x1F801128;
        } else {
            count += 0x10000;
        }
    }

    if (*(volatile u16 *)0x1F801124 & 0x200) {
        return (count - D_800A6E18) >= gTimeoutThreshold;
    } else {
        return ((count - D_800A6E18) >> 3) >= gTimeoutThreshold;
    }
}
