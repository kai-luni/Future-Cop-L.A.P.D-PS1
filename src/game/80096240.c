// Categories: Platform > MDEC | Platform > DMA
#include "game/mdec_globals.h"

#include "game/dma_globals.h"

#include "game/types.h"

extern u32 *gDmaControlMdecPtr __asm__("D_8009E9F8");
extern u32 *gDma0MemAddrPtr __asm__("D_8009E9C0");
extern u32 *gDma0BlockCtrlPtr __asm__("D_8009E9C4");
extern u32 *gMdecCommandPtr __asm__("D_8009E9F0");
extern u32 *gDma0ChannelCtrlPtr __asm__("D_8009E9C8");
extern s32 func_8009635C(void);

/**
 * @brief Starts one MDEC block transfer through DMA channel 0.
 *
 * @param arg0 Transfer descriptor; the first word is the MDEC command word,
 *        arg0 + 1 is the source address.
 * @param arg1 Block size in bytes (shifted right by 5 for the word count).
 * @return Nothing.
 *
 * @par Confirmed
 * func_8009635C() runs first (transfer-complete wait), then the channel
 * words are programmed: D_8009E9F8 |= 0x88 (control), D_8009E9C0 = arg0 + 1
 * (address), D_8009E9C4 = (arg1 >> 5) << 16 | 0x20 (block count/size),
 * D_8009E9F0 = *arg0 (MDEC command word), D_8009E9C8 = 0x01000201 (channel
 * control: start, memory-to-device direction, block mode).
 *
 * @par Likely role
 * The block mover of the movie decoder: func_80095F50 copies the
 * quantisation tables and submits two blocks, func_80095FF4 flips the decode
 * control bits, func_80096150 resets MDEC and uploads both table blocks.
 *
 * @par Unknown
 * None for the register map: D_8009E9C0..E9F8 are the DMA channel 0 group
 * (gDma0MemAddrPtr, gDma0BlockCtrlPtr, gDma0ChannelCtrlPtr) plus the MDEC
 * command port gMdecCommandPtr.
 *
 * @par Callers
 * func_80095F50, func_80095FF4, func_80096150.
 */
void gStartMdecBlockTransfer(u32 *arg0, u32 arg1) __asm__("func_80096240");

void gStartMdecBlockTransfer(u32 *arg0, u32 arg1)
{
    func_8009635C();
    *gDmaControlMdecPtr |= 0x88;
    *gDma0MemAddrPtr = (u32)(arg0 + 1);
    *gDma0BlockCtrlPtr = ((arg1 >> 5) << 16) | 0x20;
    *gMdecCommandPtr = *arg0;
    *gDma0ChannelCtrlPtr = 0x01000201;
}
