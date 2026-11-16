// Categories: Platform > Hardware Transfer
#include "game/cdrom_globals.h"

#include "game/dma_globals.h"

#include "game/types.h"

extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern volatile u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
extern volatile u32 *gCdromAccessDelayPtr __asm__("D_8009CBA0");
extern volatile u32 *gComAccessDelayPtr __asm__("D_8009CB7C");
extern volatile u32 *gDmaControlCdromPtr __asm__("D_8009CBA4");
extern volatile u32 *gDma3MemAddrPtr __asm__("D_8009CBA8");
extern volatile u32 *gDma3BlockCtrlPtr __asm__("D_8009CBAC");
extern volatile u32 *gDma3ChannelCtrlPtr __asm__("D_8009CBB0");

/**
 *  @brief Configures hardware registers and starts a transfer.
 *
 *  Programs control, destination, and size registers, then waits for readiness.
 *  The size register receives size with bit 16 set.
 *  @param destination Destination value written to the transfer register.
 *  @param size Size value combined with 0x10000 for the transfer register.
 *  @return Always 0.
 *  @par Globals
 *  D_8009CB6C and D_8009CB78 point to byte control and status registers.
 *  D_8009CBA0, D_8009CB7C, and D_8009CBA4 point to configuration registers.
 *  D_8009CBA8, D_8009CBAC, and D_8009CBB0 point to transfer registers.
 *  @par Control flow
 *  Polls bit 0x40 until set, then writes and reads the final control register.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes hardware registers and may busy-wait indefinitely.
 *  @par Callers
 *  Confirmed: func_8008E5B8.
 */
int func_8008FD30(u32 destination, u32 size)
{
    volatile u8 *p;
    volatile u32 status;

    *gCdromIndexStatusPtr = 0;
    *gCdromRequestStatusPtr = 0x80;
    *gCdromAccessDelayPtr = 0x21020843;
    *gComAccessDelayPtr = 0x1325;
    *gDmaControlCdromPtr |= 0x8000;
    *gDma3MemAddrPtr = destination;
    *gDma3BlockCtrlPtr = size | 0x10000;
    p = gCdromIndexStatusPtr;
    while ((*p & 0x40) == 0) {
        continue;
    }
    *gDma3ChannelCtrlPtr = 0x11400100;
    status = *gDma3ChannelCtrlPtr;
    return 0;
}
