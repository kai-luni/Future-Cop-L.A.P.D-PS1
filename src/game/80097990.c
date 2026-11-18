// Categories: Platform > SPU | Utilities > Timing
#include "game/types.h"

/**
 * @brief Executes a short fixed-count busy-wait delay loop for SPU hardware timing.
 *
 * Confirmed: initializes a volatile local var1 to 0xD and multiplies it by 0xD across 60 iterations (var0 = 0..0x3B) without accessing globals or returning a value. Category follows its exclusive SPU register/DMA callers (gSpuResetHardware, gSpuWriteMemBlock, func_800972F8, func_800973B4).
 *
 * @param void No parameters.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single counted for loop (0 <= var0 < 0x3C) over volatile stack variables var0 and var1.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed (consumes CPU cycles via volatile stack arithmetic).
 *
 * @par Callers
 * Confirmed: gSpuResetHardware (9 sites: 0x80096F0C..0x800970EC), gSpuWriteMemBlock (4 sites: 0x80097170..0x80097250), func_800972F8 (0x8009730C), func_800973B4 (0x800973D4, 0x800973F8) / Likely: none / Unknown: none.
 */
void gSpuBusyWait(void) __asm__("func_80097990");

void gSpuBusyWait(void)
{
    volatile s32 var0;
    volatile s32 var1;

    var1 = 0xD;
    for (var0 = 0; var0 < 0x3C; var0++) {
        var1 *= 0xD;
    }
}
