// Categories: Platform > Register State
#include "game/types.h"
extern volatile u32 D_8009EE90;
extern volatile u16 D_800A6EA0[];
extern volatile u16 table[] __asm__("D_800A6EA0");
extern volatile u16 *D_8009EEA4;
extern volatile u32 D_8009EA5C;

/**
 *  @brief Reads or modifies a 24-bit value stored across two halfword entries.
 *
 *  Selects a volatile table and combines the low halfword with the low byte
 *  of the high entry; supported modes set, clear, or replace the value.
 *  @param mode Operation: 1 sets bits, 0 clears bits, 8 replaces, others read only.
 *  @param bits Bit mask or replacement value; only its low 24 bits are returned.
 *  @param low Halfword index containing the low 16 bits.
 *  @param high Halfword index containing the upper 8 bits.
 *  @return The resulting 24-bit value, or the original value for other modes.
 *  @par Globals
 *  D_8009EE90 selects the table; D_800A6EA0 is the local table and
 *  D_8009EEA4 points to the alternate table. D_8009EA5C records local changes.
 *  @par Control flow
 *  Reads both entries, then dispatches on mode; local writes mark a bit derived from low.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Supported modes write volatile entries and may update D_8009EA5C.
 *  @par Callers
 *  Confirmed: func_80098788.
 */
u32 func_800987B8(s32 mode, u32 bits, s32 low, s32 high)
{
    register s32 command __asm__("$9") = mode;
    register u32 value __asm__("$8") = bits;
    register u32 result __asm__("$10");
    register volatile u16 *base __asm__("$4");
    register u32 flag __asm__("$2");
    register u32 upper __asm__("$3");
    register volatile u16 *ptr __asm__("$2");
    register u32 lower __asm__("$2");
    register u32 masked __asm__("$2");
    __asm__("" : "+r"(command));
    flag = D_8009EE90;
    __asm__("" : "+r"(flag));
    flag &= 1;
    __asm__("" : "+r"(flag));
    base = D_800A6EA0;
    if (!flag) base = D_8009EEA4;
    ptr = (volatile u16 *)((high << 1) + (u32)base);
    __asm__("" : "+r"(ptr));
    upper = *ptr;
    __asm__("" : "+r"(upper));
    __asm__("" : "+r"(low) : "r"(upper));
    ptr = (volatile u16 *)((low << 1) + (u32)base);
    __asm__("" : "+r"(ptr));
    lower = *ptr;
    __asm__("" : "+r"(lower));
    result = lower | ((upper & 0xff) << 16);
    __asm__("" : "+r"(low), "+r"(high) : : "$11");
    switch (command) {
    case 1: {
        register s32 offset __asm__("$3") = low << 1;
        if (D_8009EE90 & 1) {
            *(volatile u16 *)((char *)table + offset) |= value;
            table[high] |= (value >> 16) & 0xff;
            D_8009EA5C |= 1 << ((low - 0xc6) >> 1);
        } else {
            register volatile u16 *other __asm__("$5") = D_8009EEA4;
            *(volatile u16 *)(offset + (u32)other) |= value;
            other[high] |= (value >> 16) & 0xff;
        }
        masked = value & 0xffffff;
        __asm__("" : "+r"(masked));
        result |= masked;
        break;
    }
    case 0: {
        register s32 offset __asm__("$3") = low << 1;
        if (D_8009EE90 & 1) {
            *(volatile u16 *)(offset + (u32)table) &= ~value;
            table[high] &= ~((value >> 16) & 0xff);
            D_8009EA5C |= 1 << ((low - 0xc6) >> 1);
        } else {
            register volatile u16 *other __asm__("$5") = D_8009EEA4;
            *(volatile u16 *)(offset + (u32)other) &= ~value;
            other[high] &= ~((value >> 16) & 0xff);
        }
        masked = value & 0xffffff;
        __asm__("" : "+r"(masked));
        result &= ~masked;
        break;
    }
    case 8: {
        register s32 offset __asm__("$3") = high << 1;
        if (D_8009EE90 & 1) {
            table[low] = value;
            *(volatile u16 *)(offset + (u32)table) = (value >> 16) & 0xff;
            D_8009EA5C |= 1 << ((low - 0xc6) >> 1);
        } else {
            register s32 looff __asm__("$2") = low << 1;
            register volatile u16 *other __asm__("$4");
            __asm__("" : "+r"(looff));
            other = D_8009EEA4;
            __asm__("" : "+r"(other));
            *(volatile u16 *)(looff + (u32)other) = value;
            *(volatile u16 *)(offset + (u32)other) = (value >> 16) & 0xff;
        }
        masked = 0xffffff;
        __asm__("" : "+r"(masked));
        result = value & masked;
        break;
    }
    }
    return result & 0xffffff;
}
