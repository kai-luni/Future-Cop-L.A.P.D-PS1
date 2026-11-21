// Categories: Platform > Audio
#include "game/spu_globals.h"

#include "game/types.h"
extern u32 gSpuAlignMask __asm__("D_8009EED4");
extern u32 gSpuAlignBase __asm__("D_8009EED0");
extern u32 gSpuAlignEnable __asm__("D_8009EEC8");
extern u32 gSpuAddrShift __asm__("D_8009EECC");
extern u16 *gSpuRegBasePtr __asm__("D_8009EEA4");
u32 gSpuEncodeOrWriteAddr(s32 reg, u32 addr) __asm__("func_80097808");


/**
 *  @brief Aligns and encodes an address, optionally writing it to a register.
 *
 *  Uses the configured alignment and shift to form a 16-bit register value;
 *  special register selectors return the encoded index or adjusted address.
 *  @param reg Register index, or -1 for the index and -2 for the address.
 *  @param addr Address to align and encode.
 *  @return The low 16 bits of the shifted address for -1, otherwise the adjusted address.
 *  @par Globals
 *  D_8009EEC8 enables alignment; D_8009EED0 and D_8009EED4 define its calculation.
 *  D_8009EECC supplies the shift; D_8009EEA4 points to the register base.
 *  @par Control flow
 *  Aligns only when enabled and unaligned, then dispatches on reg; zero alignment traps.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  For other selectors, writes the truncated shifted address to the selected register.
 *  @par Callers
 *  Confirmed: func_80098BB8, func_80098EE8, gSpuSetVoiceAttributes, gSpuSetVoiceAttributesOne.
 */
u32 gSpuEncodeOrWriteAddr(s32 reg, u32 addr)
{
    register s32 saved_reg __asm__("$6") = reg;
    register u32 align __asm__("$4");
    register u32 shifted __asm__("$7");
    register u32 shift __asm__("$2");
    register u16 *base __asm__("$4");
    register u16 *dest __asm__("$3");
    u32 index, rem;

    if (gSpuAlignEnable != 0) {
        align = gSpuAlignBase;
        __asm__ (
            "divu\t$zero,%1,%2\n\t"
            "bnez\t%2,1f\n\t"
            "nop\n\t"
            ".word\t0x0007000d\n"
            "1:\n\t"
            "mfhi\t%0"
            : "=r" (rem)
            : "r" (addr), "r" (align)
        );
        if (rem != 0) {
            addr += align;
            addr &= ~gSpuAlignMask;
        }
    }
    shift = gSpuAddrShift;
    __asm__("" : "=r" (shift) : "0" (shift));
    shifted = addr >> shift;
    __asm__("" : "=r" (shifted) : "0" (shifted));
    index = shifted;
    switch (saved_reg) {
    case -1: return index & 0xFFFF;
    case -2: return addr;
    default:
        base = gSpuRegBasePtr;
        dest = (u16 *)((u32)(saved_reg * 2) + (u32)base);
        __asm__("" : "=r" (dest) : "0" (dest));
        *dest = shifted;
        return addr;
    }
}
