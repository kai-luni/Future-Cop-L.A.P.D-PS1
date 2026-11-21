// Categories: Platform > MDEC | Platform > DMA
#include "game/mdec_globals.h"

#include "game/types.h"

extern u32 *gMdecControlStatusPtr __asm__("D_8009E9F4");
extern u8 gTimeoutLogMsg[] __asm__("D_80012BA8");
extern void gResetMdecOnTimeout(void *) __asm__("func_8009649C");

/**
 * @brief Waits for the PS1 MDEC input FIFO/busy status bit to clear with timeout recovery.
 *
 * Polls bit 29 (0x20000000) of the MDEC status register (0x1F801824) up to 0x100000
 * iterations; on timeout, triggers MDEC/DMA reset with label "MDEC_in_sync" and returns -1.
 *
 * @param void No parameters.
 * @return 0 when MDEC input status clears, or -1 if the 0x100000-step poll times out.
 *
 * @par Globals
 * D_8009E9F4 - pointer to MDEC1 status register (0x1F801824); D_80012BA8 - "MDEC_in_sync".
 *
 * @par Control flow
 * Checks *D_8009E9F4 & 0x20000000; if set, decrements volatile count from 0x100000 until clear or -1, calling func_8009649C on timeout.
 *
 * @par Calls
 * func_8009649C - prints timeout message and resets MDEC and DMA0/DMA1.
 *
 * @par Side effects
 * Polls hardware register 0x1F801824; on timeout resets MDEC and DMA channels 0 and 1.
 *
 * @par Callers
 * Confirmed: func_80096090 (mode 0 sync), gStartMdecBlockTransfer (before starting MDEC-in DMA0 transfer).
 */
s32 gMdecWaitInSync(void) __asm__("func_8009635C");

s32 gMdecWaitInSync(void)
{
    volatile s32 count = 0x100000;

    if (*gMdecControlStatusPtr & 0x20000000) {
        do {
            count = count - 1;
            if (count == -1) {
                register u8 *message __asm__("$4");
                __asm__ volatile("lui\t%0,%%hi(D_80012BA8)" : "=r"(message));
                gResetMdecOnTimeout(message + 0x2BA8);
                return -1;
            }
        } while (*gMdecControlStatusPtr & 0x20000000);
    }
    return 0;
}
