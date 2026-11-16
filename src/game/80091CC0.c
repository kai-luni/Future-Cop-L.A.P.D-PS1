// Categories: Platform > DMA | Platform > Interrupts
#include "game/dma_globals.h"

#include "game/types.h"

extern s32 *gDmaInterruptCtrlPtr __asm__("D_8009DD60");
extern s32 D_8009DD64[8];

extern void func_80091F38(s32 *destination, u32 count);
extern void func_800914B0(s32 a0, void *a1);
extern void func_80091D0C(void);
extern void func_80091E8C(s32 a0, s32 a1);

/**
 * @brief Initializes the DMA interrupt handler table, clears DICR, and returns the DMA callback setter.
 *
 * Zeroes the 8-entry DMA channel callback table D_8009DD64 via func_80091F38, clears the DICR
 * register at *D_8009DD60, registers func_80091D0C as IRQ 3 handler via func_800914B0, and returns func_80091E8C.
 *
 * @param void No parameters.
 * @return Function pointer to func_80091E8C (per-channel DMA callback installer stored in D_8009DD10->field4).
 *
 * @par Globals
 * D_8009DD64 - 8-slot DMA callback pointer array (zeroed); D_8009DD60 - pointer to DICR (0x1F8010F4, cleared to 0).
 *
 * @par Control flow
 * Straight-line initialization sequence returning func_80091E8C.
 *
 * @par Calls
 * func_80091F38 - zeroes 8 words in D_8009DD64; func_800914B0 - registers func_80091D0C at IRQ slot 3.
 *
 * @par Side effects
 * Clears D_8009DD64 and *D_8009DD60, and installs func_80091D0C in the interrupt controller dispatch table.
 *
 * @par Callers
 * Confirmed: func_80091614 (0x800916B8, during interrupt/callback system initialization) / Likely: none / Unknown: none.
 */
void *func_80091CC0(void)
{
    func_80091F38(D_8009DD64, 8);
    *gDmaInterruptCtrlPtr = 0;
    func_800914B0(3, func_80091D0C);
    return func_80091E8C;
}
