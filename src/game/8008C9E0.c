// Categories: Platform > Interrupts | Platform > CPU
#include "game/types.h"
void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");


/**
 * @brief Leaves a critical section: enables interrupts.
 *
 * Reads COP0 status ($12), sets 0x401 (interrupt enable plus the current-enable
 * bit) and writes it back. Because the previous state is not restored, this
 * always ends up with interrupts enabled - the exact opposite mask of
 * func_8008C9C0.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_800861E4, right after the callback set-up.
 *
 * @par Role and path
 * Unconditionally enables CPU interrupts by reading COP0 status register $12,
 * ORing 0x401, and writing the result back to $12. Called by func_800861E4
 * after callback setup.
 */
void gEnableCpuInterrupts(void)
{
    __asm__ volatile (
        "mfc0 $4, $12\n\t"
        "nop\n\t"
        "ori $4, $4, 0x401\n\t"
        "mtc0 $4, $12\n\t"
        "nop"
        :
        :
        : "$4"
    );
}
