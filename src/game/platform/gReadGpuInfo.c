// Categories: Platform > Register Access
#include "game/gpu_globals.h"

#include "game/types.h"

extern u32 *gGpuGp0PortPtr __asm__("D_8009DEC4");
extern u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern void gSubmitGpuDmaJob(s32, s32, s32, s32) __asm__("func_80094A3C");
u32 gReadGpuInfo(u32 arg0) __asm__("func_800949E8");


/**
 *  @brief Writes a tagged value and reads a masked result.
 *
 *  Sets bit 28 in the argument before writing through one global pointer.
 *  The value read through the other pointer is limited to its low 24 bits.
 *  @param arg0 Value to tag and write.
 *  @return Low 24 bits of the value read through D_8009DEC4.
 *  @par Globals
 *  D_8009DEC8 supplies the write address; D_8009DEC4 supplies the read address.
 *  @par Control flow
 *  Performs the write before the read, without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes one 32-bit value through D_8009DEC8.
 *  @par Callers
 *  Confirmed: func_8009423C.
 */
u32 gReadGpuInfo(u32 arg0)
{
    u32 value = arg0 | 0x10000000;

    *gGpuGp1PortPtr = value;
    return *gGpuGp0PortPtr & 0xFFFFFF;
}
