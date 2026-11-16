// Categories: Platform > MDEC | Graphics > Video Decode
#include "game/dma_globals.h"

#include "game/types.h"

extern u32 *D_8009E9F4;
extern u32 *D_8009E9C8;
extern volatile u32 *gDma1ChannelCtrlPtr __asm__("D_8009E9D4");
extern u32 D_8009E8B0[];
extern u32 D_8009E934[];
extern u8 D_80010000[];
extern void func_80096240(u32 *, u32);
extern void func_8008E000(void *, s32);

/**
 * @brief MDEC reset/init for movie playback: option 0 resets and uploads both quant-table command blocks, option 1 resets only, anything else prints "MDEC_rest:bad option(%d)".
 *
 * @param option 0 = full init, 1 = reset only, other = error print.
 *
 * @par Globals
 * D_8009E9F4 - pointer to MDEC control 0x1F801824: 0x80000000 resets, 0x60000000 enables data input and output. D_8009E9C8/D_8009E9D4 - DMA0/DMA1 channel control (0x1F801088/0x1F801098), cleared to stop MDECin/MDECout. D_8009E8B0/D_8009E934 - command blocks headed 0x40000001 (Y quant tables) and 0x60000000 (color quant tables).
 *
 * @par Control flow
 * Three-way switch; option 1 adds a volatile read-back of the DMA1 control word after clearing it.
 *
 * @par Calls
 * func_80096240 - uploads each command block through MDECin DMA (option 0 only). func_8008E000 - printf-style error output with the format string at D_80010000+0x2B8C (default case).
 *
 * @par Side effects
 * Writes MDEC control and DMA channel registers; starts two DMA command uploads on option 0.
 *
 * @par Callers
 * Confirmed: func_80095E90 (forwards the movie option code; the STR path uses 0 at start and 1 after frame decode).
 */
void func_80096150(s32 option)
{
    u32 *p;
    register u32 *q __asm__("$2");
    switch (option) {
    case 0:
        p = D_8009E9F4;
        __asm__ volatile ("" : : "r"(p));
        *p = 0x80000000;
        q = D_8009E9C8;
        __asm__ volatile ("" : : "r"(q) : "$4");
        *q = 0;
        *gDma1ChannelCtrlPtr = 0;
        *D_8009E9F4 = 0x60000000;
        func_80096240(D_8009E8B0, 0x20);
        func_80096240(D_8009E934, 0x20);
        break;
    case 1:
        *D_8009E9F4 = 0x80000000;
        *D_8009E9C8 = 0;
        *gDma1ChannelCtrlPtr = 0;
        (void)*gDma1ChannelCtrlPtr;
        *D_8009E9F4 = 0x60000000;
        break;
    default:
        __asm__ volatile ("" : : : "$4");
        func_8008E000(D_80010000 + 0x2b8c, option);
        break;
    }
}
