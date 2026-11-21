// Categories: Platform > Sound > DMA | Platform > Interrupts | BIOS > System Calls
#include "game/types.h"

typedef void (*SpuDmaCallback)(void);

extern s32 gSecondCallGuard __asm__("D_8009EE94");
extern s32 gSpuTransferCallback __asm__("D_8009EEDC");
extern SpuDmaCallback gInstalledHandler __asm__("D_8009EEE0");
extern void *gSpuDmaCallback __asm__("D_8009EA2C");

extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void gSpuSetDmaCallback(SpuDmaCallback callback) __asm__("func_800979F8");
extern void gBiosCloseEvent(void *arg) __asm__("func_8008C970");
extern void gBiosDisableEvent(void *arg) __asm__("func_8008C990");

/**
 * @brief Tears down the SPU DMA completion event and clears its driver callback inside a critical section.
 *
 * Counterpart to gSpuInitDmaEvent: if D_8009EE94 is 1, clears D_8009EE94, enters a critical section, zeroes D_8009EEDC and D_8009EEE0, clears the channel-4 SPU DMA callback via gSpuSetDmaCallback(0), closes and disables BIOS event D_8009EA2C, and exits the critical section.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009EE94 - init guard cleared to 0; D_8009EEDC/D_8009EEE0 - SPU DMA state/callback words cleared; D_8009EA2C - BIOS event handle.
 *
 * @par Control flow
 * Guarded by D_8009EE94 == 1; straight-line teardown inside Enter/ExitCriticalSection.
 *
 * @par Calls
 * func_8008C9A0/func_8008C9B0 - critical section (syscall 0 codes 1/2); gSpuSetDmaCallback - clear channel-4 SPU DMA callback; func_8008C970 - BIOS B0:09 (CloseEvent); func_8008C990 - BIOS B0:0D (DisableEvent).
 *
 * @par Side effects
 * Zeroes D_8009EE94/D_8009EEDC/D_8009EEE0, disarms SPU DMA callback, and closes/disables BIOS event D_8009EA2C.
 *
 * @par Callers
 * Confirmed: func_80021C80.
 */
void gSpuShutdownDmaEvent(void) __asm__("func_80097A48");

void gSpuShutdownDmaEvent(void)
{
    if (gSecondCallGuard == 1) {
        __asm__ volatile(
            ".set\tnoreorder\n\t"
            "lui\t$at,%hi(D_8009EE94)\n\t"
            "jal\tfunc_8008C9A0\n\t"
            "sw\t$zero,%lo(D_8009EE94)($at)\n\t"
            "addu\t$a0,$zero,$zero\n\t"
            "lui\t$at,%hi(D_8009EEDC)\n\t"
            "sw\t$zero,%lo(D_8009EEDC)($at)\n\t"
            "lui\t$at,%hi(D_8009EEE0)\n\t"
            "jal\tfunc_800979F8\n\t"
            "sw\t$zero,%lo(D_8009EEE0)($at)\n\t"
            ".set\treorder\n\t"
        );
        gBiosCloseEvent(gSpuDmaCallback);
        gBiosDisableEvent(gSpuDmaCallback);
        gExitCriticalSection();
    }
}
