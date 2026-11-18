// Categories: Platform > MDEC | Platform > DMA
#include "game/dma_globals.h"

#include "game/types.h"

extern u32 *gDma1ChannelCtrlPtr __asm__("D_8009E9D4");
extern u8 gMdecStatusLogMsg[] __asm__("D_80012BB8");
extern void func_8009649C(void *);

/**
 * @brief Waits for the PS1 MDEC-out DMA1 channel busy bit to clear with timeout recovery.
 *
 * Polls bit 24 (0x01000000) of DMA1 channel control (0x1F801098) up to 0x100000
 * iterations; on timeout, triggers MDEC/DMA reset with label "MDEC_out_sync" and returns -1.
 *
 * @param void No parameters.
 * @return 0 when DMA1 busy clears, or -1 if the 0x100000-step poll times out.
 *
 * @par Globals
 * D_8009E9D4 - pointer to DMA1 channel control register (0x1F801098); D_80012BB8 - "MDEC_out_sync".
 *
 * @par Control flow
 * Checks *D_8009E9D4 & 0x01000000; if set, decrements volatile count from 0x100000 until clear or -1, calling func_8009649C on timeout.
 *
 * @par Calls
 * func_8009649C - prints timeout message and resets MDEC and DMA0/DMA1.
 *
 * @par Side effects
 * Polls hardware register 0x1F801098; on timeout resets MDEC and DMA channels 0 and 1.
 *
 * @par Callers
 * Confirmed: func_800960CC (mode 0 sync), func_800962D0 (before starting MDEC-out DMA1 transfer).
 */
s32 gMdecWaitOutSync(void) __asm__("func_800963F0");

s32 gMdecWaitOutSync(void)
{
    volatile s32 count = 0x100000;

    if (*gDma1ChannelCtrlPtr & 0x01000000) {
        do {
            count = count - 1;
            if (count == -1) {
                register u8 *message __asm__("$4");
                __asm__ volatile("lui\t%0,%%hi(D_80012BB8)" : "=r"(message));
                func_8009649C(message + 0x2BB8);
                return -1;
            }
        } while (*gDma1ChannelCtrlPtr & 0x01000000);
    }
    return 0;
}
