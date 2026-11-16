// Categories: Platform > Hardware Synchronization
#include "game/intc_globals.h"

#include "game/types.h"

extern u32 *gInterruptStatusPtr __asm__("D_8009C788");
extern u8 *D_8009C78C;

extern s32 func_8008C8B0(void);

/**
 *  @brief Waits for a status bit to clear and then sets a control bit.
 *
 *  Initializes a shared counter and polls a volatile status field, using a
 *  helper to detect an early-exit condition while the status bit remains set.
 *  @return Zero on helper-reported early exit, or one after setting the bit.
 *  @par Globals
 *  D_8009C788 points to the counter; D_8009C78C points to the status and
 *  control fields at offsets 4 and 0xA.
 *  @par Control flow
 *  While status bit 0x80 is set, calls the helper and rechecks the bit;
 *  a nonzero helper result returns immediately without setting control bit 0x10.
 *  @par Calls
 *  func_8008C8B0 supplies the early-exit condition during polling.
 *  @par Side effects
 *  Writes 0xFFFFFF7F to the counter and, on success, sets control bit 0x10.
 *  @par Callers
 *  Confirmed: func_8008A8E0, func_8008BA34.
 */
s32 func_8008ADB4(void)
{
    u32 *counter = gInterruptStatusPtr;
    u8 *state = D_8009C78C;

    *counter = 0xFFFFFF7F;

    if (*(volatile u16 *)(state + 4) & 0x80) {
        for (;;) {
            if (func_8008C8B0()) {
                return 0;
            }
            if (!(*(volatile u16 *)(D_8009C78C + 4) & 0x80)) {
                break;
            }
        }
    }

    *(volatile u16 *)(D_8009C78C + 0xA) |= 0x10;

    return 1;
}
