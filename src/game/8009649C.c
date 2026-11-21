// Categories: Platform > MDEC | Platform > DMA
#include "game/mdec_globals.h"

#include "game/dma_globals.h"

#include "game/types.h"

extern u8 gMdecTimeoutFormat __asm__("D_80012BC8");
extern volatile u32 *gMdecControlStatusPtr __asm__("D_8009E9F4");
extern volatile u32 *gDma0ChannelCtrlPtr __asm__("D_8009E9C8");
extern volatile u32 *gDma1ChannelCtrlPtr __asm__("D_8009E9D4");
extern void gBiosPrintf(void *arg0, s32 arg1) __asm__("func_8008E000");
s32 gResetMdecOnTimeout(s32 arg0) __asm__("func_8009649C");


/**
 * @brief Logs an MDEC/DMA timeout and resets the MDEC unit and DMA channels 0 and 1.
 *
 * Prints D_80012BC8 ('%s timeout:\n') with arg0 via func_8008E000, writes MDEC reset (0x80000000) to 0x1F801824, zeroes DMA0 MDECin (0x1F801088) and DMA1 MDECout (0x1F801098) control registers, reads back DMA1 CHCR, and sets MDEC1 control to 0x60000000.
 *
 * @param arg0 Caller label string passed to printf (e.g. 'MDEC_in_sync' or 'MDEC_out_sync').
 * @return Always 0.
 *
 * @par Globals
 * D_80012BC8 - '%s timeout:\n' format string; D_8009E9F4 - MDEC1 register (0x1F801824); D_8009E9C8 - DMA0 CHCR (0x1F801088); D_8009E9D4 - DMA1 CHCR (0x1F801098).
 *
 * @par Control flow
 * Straight-line sequence of console output and MMIO writes; no branches or loops.
 *
 * @par Calls
 * func_8008E000 - BIOS printf for timeout diagnostic.
 *
 * @par Side effects
 * Prints diagnostic string, resets MDEC hardware (0x1F801824), and halts DMA channels 0 and 1.
 *
 * @par Callers
 * Confirmed: func_8009635C, func_800963F0.
 */
s32 gResetMdecOnTimeout(s32 arg0)
{
    register s32 result __asm__("$2");
    gBiosPrintf(&gMdecTimeoutFormat, arg0);
    *gMdecControlStatusPtr = 0x80000000;
    *gDma0ChannelCtrlPtr = 0;
    *gDma1ChannelCtrlPtr = 0;
    __asm__ volatile ("" : : : "memory");
    result = 0;
    __asm__ volatile ("" : "=r"(result) : "0"(result));
    (void)*gDma1ChannelCtrlPtr;
    *gMdecControlStatusPtr = 0x60000000;
    return result;
}
