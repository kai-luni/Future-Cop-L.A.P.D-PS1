// Categories: Platform > Interrupts
#include "game/intc_globals.h"

#include "game/types.h"

extern u16 *gInterruptMaskPtr __asm__("D_8009DD18");
u16 gSwapIrqMask(u16 a0) __asm__("func_800915FC");


/**
 * @brief Atomically swaps the 16-bit PS1 interrupt mask register (I_MASK) via D_8009DD18.
 *
 * Reads the current 16-bit value at *D_8009DD18 (0x1F801074, hardware I_MASK), writes a0 to it, and returns the previous mask so callers can disable and restore hardware interrupts.
 *
 * @param a0 New 16-bit interrupt mask value (0 disables all masked IRQ lines).
 * @return Previous 16-bit interrupt mask read from *D_8009DD18.
 *
 * @par Globals
 * D_8009DD18 - pointer to PS1 hardware I_MASK register (0x1F801074).
 *
 * @par Control flow
 * Straight-line halfword load, halfword store, and return; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites the PS1 hardware I_MASK register at 0x1F801074.
 *
 * @par Callers
 * Confirmed: func_80094A3C (0x80094AA8, 0x80094B50, 0x80094CA4), func_80094CEC (0x80094D18, 0x80094EA4), func_80094F4C (0x80094F5C, 0x80095070), func_8009520C (0x800952B0, 0x8009532C).
 *
 * @par Role and path
 * Reads the current 16-bit PS1 I_MASK register (0x1F801074) through
 * D_8009DD18, writes a0 to it, and returns the previous mask. Called by
 * func_80094A3C, func_80094CEC, func_80094F4C, and func_8009520C to disable
 * and restore hardware interrupts.
 */
u16 gSwapIrqMask(u16 a0)
{
    u16 old = *gInterruptMaskPtr;
    *gInterruptMaskPtr = a0;
    return old;
}
