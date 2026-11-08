#include "game/types.h"
extern u32 D_8009EEC8, D_8009EED0, D_8009EED4, D_8009EECC;
extern u16 *D_8009EEA4;

u32 func_80097808(s32 reg, u32 addr)
{
    register s32 saved_reg __asm__("$6") = reg;
    register u32 align __asm__("$4");
    register u32 shifted __asm__("$7");
    register u32 shift __asm__("$2");
    register u16 *base __asm__("$4");
    register u16 *dest __asm__("$3");
    u32 index, rem;

    if (D_8009EEC8 != 0) {
        align = D_8009EED0;
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
            addr &= ~D_8009EED4;
        }
    }
    shift = D_8009EECC;
    __asm__("" : "=r" (shift) : "0" (shift));
    shifted = addr >> shift;
    __asm__("" : "=r" (shifted) : "0" (shifted));
    index = shifted;
    switch (saved_reg) {
    case -1: return index & 0xFFFF;
    case -2: return addr;
    default:
        base = D_8009EEA4;
        dest = (u16 *)((u32)(saved_reg * 2) + (u32)base);
        __asm__("" : "=r" (dest) : "0" (dest));
        *dest = shifted;
        return addr;
    }
}
