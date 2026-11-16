// Categories: Platform > DMA | Graphics > Video Decode
#include "game/dma_globals.h"

#include "game/types.h"

extern u32 *D_8009E9CC;
extern u32 *D_8009E9D0;
extern u32 *gDma1ChannelCtrlPtr __asm__("D_8009E9D4");
extern u32 *D_8009E9F8;

extern void func_800963F0(s32 destination, u32 size);

/**
 * @brief Starts a block transfer into the decode buffer.
 *
 * @param destination Target address.
 * @param size Byte count; stored as ((size >> 5) << 16) | 0x20 in the descriptor.
 * @return Nothing.
 *
 * @par Confirmed sequence
 * 1. func_800963F0() waits (with timeout) until bit 0x01000000 of *D_8009E9D4
 *    clears.
 * 2. *D_8009E9F8 |= 0x88 (two control bits set), *D_8009E9D4 = 0.
 * 3. *D_8009E9CC = destination, *D_8009E9D0 = ((size >> 5) << 16) | 0x20,
 *    *D_8009E9D4 = 0x01000200 - the command word that starts the transfer.
 *
 * @par Likely role
 * The arming step of the DMA/CD transfer: the two words at E9CC/E9D0 are the
 * address and the count (upper half = 32-byte units), 0x20 the low part of the
 * count/command field.
 *
 * @par Callers
 * func_80096070 (the sized image copy).
 */
void func_800962D0(s32 destination, u32 size)
{
    func_800963F0(destination, size);
    *D_8009E9F8 |= 0x88;
    *gDma1ChannelCtrlPtr = 0;
    *D_8009E9CC = destination;
    *D_8009E9D0 = ((size >> 5) << 16) | 0x20;
    *gDma1ChannelCtrlPtr = 0x01000200;
}
