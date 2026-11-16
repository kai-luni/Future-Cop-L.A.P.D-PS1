// Categories: Platform > DMA
#include "game/dma_globals.h"

#include "game/types.h"

__asm__(".macro move dst, src\n\taddu \\dst, \\src, $zero\n.endm\n");

extern u32 *gDmaControlGpuPtr __asm__("D_8009DEE4");
extern u32 *gDma6ChannelCtrlPtr __asm__("D_8009DEE0");
extern u32 *gDma6MemAddrPtr __asm__("D_8009DED8");
extern u32 *gDma6BlockCtrlPtr __asm__("D_8009DEDC");

extern void func_800951D8(void);
extern s32 func_8009520C(void);

/**
 *  @brief Starts a register-controlled transfer and waits for completion.
 *
 *  The transfer address is set to the last word of the supplied buffer,
 *  and the count and control registers are programmed before polling.
 *  @param arg0 Base address of the transfer buffer.
 *  @param arg1 Transfer count in words.
 *  @return The count on completion, or -1 if the polling helper reports failure.
 *  @par Globals
 *  D_8009DEE4 enables a control bit; D_8009DEE0 controls and reports the transfer.
 *  D_8009DED8 receives the address; D_8009DEDC receives the count.
 *  @par Control flow
 *  Polls while the active bit is set, returning early on helper failure.
 *  @par Calls
 *  func_800951D8 runs after startup; func_8009520C checks during polling.
 *  @par Side effects
 *  Writes the pointed-to registers and initiates a transfer.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8009415C(u8 *arg0, s32 arg1)
{
    *gDmaControlGpuPtr |= 0x8000000;
    *gDma6ChannelCtrlPtr = 0;
    *gDma6MemAddrPtr = (u32 *)(arg0 + ((arg1 << 2) - 4));
    *gDma6BlockCtrlPtr = arg1;
    *gDma6ChannelCtrlPtr = 0x11000002;
    func_800951D8();
    if (*gDma6ChannelCtrlPtr & 0x1000000) {
        do {
            if (func_8009520C() != 0) {
                return -1;
            }
        } while (*gDma6ChannelCtrlPtr & 0x1000000);
    }
    return arg1;
}
