// Categories: Platform > DMA Interrupts
#include "game/dma_globals.h"

typedef unsigned int u32;

typedef struct DmaChannel {
    u32 madr;
    u32 bcr;
    u32 chcr;
    u32 pad;
} DmaChannel;

extern volatile u32 *gDmaInterruptCtrlPtr __asm__("D_8009DD60");
extern  void (*gDmaCallbackTable[])(void) __asm__("D_8009DD64");
extern DmaChannel *gDmaChannelRegBasePtr __asm__("D_8009DD84");

extern char D_8001286C[];
extern char D_80012888[];

extern void func_8008E000(char *, ...);

/**
 *  @brief Dispatches pending DMA channel callbacks and reports exceptional status.
 *
 *  The handler repeatedly reads the seven channel status bits, writes the
 *  corresponding acknowledgement mask, and invokes registered callbacks.
 *  It then checks the control register for two exceptional status conditions.
 *  @par Globals
 *  D_8009DD60 points to the control register; D_8009DD64 holds callbacks.
 *  D_8009DD84 supplies channel addresses; D_8001286C and D_80012888 are formats.
 *  @par Control flow
 *  Scans up to seven bits per pass and repeats until no channel bits remain.
 *  An exceptional status triggers one register report and seven channel reports.
 *  @par Calls
 *  func_8008E000 prints the control value and channel memory addresses.
 *  Registered non-null callbacks are invoked indirectly.
 *  @par Side effects
 *  Writes the control register and may invoke callbacks or emit diagnostics.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80091D0C(void)
{
    int s0;
    u32 s1;

    s1 = (*gDmaInterruptCtrlPtr >> 24) & 0x7F;
    while (s1 != 0) {
        for (s0 = 0; s1 != 0 && s0 < 7; s0++, s1 >>= 1) {
            if (s1 & 1) {
                *gDmaInterruptCtrlPtr &= (1 << (s0 + 24)) | 0xFFFFFF;
                if (gDmaCallbackTable[s0] != 0) {
                    gDmaCallbackTable[s0]();
                }
            }
        }
        s1 = (*gDmaInterruptCtrlPtr >> 24) & 0x7F;
    }

    if ((*gDmaInterruptCtrlPtr & 0xFF000000) == 0x80000000 || (*gDmaInterruptCtrlPtr & 0x8000)) {
        func_8008E000(D_8001286C, *gDmaInterruptCtrlPtr);
        for (s0 = 0; s0 < 7; s0++) {
            func_8008E000(D_80012888, s0, gDmaChannelRegBasePtr[s0].madr);
        }
    }
    __asm__ volatile (".set reorder\n\t.set\tnoreorder");
}
