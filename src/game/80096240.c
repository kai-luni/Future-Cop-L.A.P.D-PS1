// Categories: Platform > Sound | Platform > DMA
#include "game/dma_globals.h"

#include "game/types.h"

extern u32 *D_8009E9F8;
extern u32 *D_8009E9C0;
extern u32 *D_8009E9C4;
extern u32 *D_8009E9F0;
extern u32 *gDma0ChannelCtrlPtr __asm__("D_8009E9C8");
extern s32 func_8009635C(void);

/**
 * @brief Starts one SPU DMA block transfer through the channel registers.
 *
 * @param arg0 Transfer descriptor; the first word is the SPU destination
 *        header, arg0 + 1 is the source address.
 * @param arg1 Block size in bytes (shifted right by 5 for the word count).
 * @return Nothing.
 *
 * @par Confirmed
 * func_8009635C() runs first (transfer-complete wait), then the channel
 * words are programmed: D_8009E9F8 |= 0x88 (control), D_8009E9C0 = arg0 + 1
 * (address), D_8009E9C4 = (arg1 >> 5) << 16 | 0x20 (block count/size),
 * D_8009E9F0 = *arg0 (header), D_8009E9C8 = 0x01000201 (channel control:
 * start, to-SPU direction, linked mode).
 *
 * @par Likely role
 * The block mover of the sound stream: the sample upload paths
 * func_80095F50/func_80095FF4/func_80096150 queue their SPU transfers here.
 *
 * @par Unknown
 * The exact register map behind D_8009E9C0..E9F8 (likely DMA channel 4).
 *
 * @par Callers
 * func_80095F50, func_80095FF4, func_80096150.
 */
void func_80096240(u32 *arg0, u32 arg1)
{
    func_8009635C();
    *D_8009E9F8 |= 0x88;
    *D_8009E9C0 = (u32)(arg0 + 1);
    *D_8009E9C4 = ((arg1 >> 5) << 16) | 0x20;
    *D_8009E9F0 = *arg0;
    *gDma0ChannelCtrlPtr = 0x01000201;
}
