// Categories: Platform > Hardware
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

#include "game/types.h"

extern u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 *D_8009DECC;
extern u32 *gDma2BlockCtrlPtr __asm__("D_8009DED0");
extern u32 *D_8009DED4;

/**
 *  @brief Writes a value and three constants through four global pointers.
 *
 *  The ordered writes configure the locations referenced by the global pointers;
 *  the source does not identify the underlying hardware or protocol.
 *  @param value Word written through the second pointer.
 *  @par Globals
 *  D_8009DEC8, D_8009DECC, D_8009DED0, and D_8009DED4 provide
 *  the four destination addresses.
 *  @par Control flow
 *  Writes 0x04000002, value, zero, and 0x01000401 in that order.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the four pointed-to words.
 *  @par Callers
 *  Confirmed: func_8009423C.
 */
void func_800949A0(u32 value)
{
    *gGpuGp1PortPtr = 0x04000002;
    *D_8009DECC = value;
    *gDma2BlockCtrlPtr = 0;
    *D_8009DED4 = 0x01000401;
}
