// Categories: Utilities > Numeric Conversion
#include "game/types.h"
u32 gConvertSpuPitchToNote(u16 arg0, u16 arg1, u16 arg2) __asm__("func_80099B2C");


/**
 *  @brief Converts a 16-bit value into a packed coarse and fractional result.
 *
 *  Searches fixed-point intervals derived from the highest set bit of arg2.
 *  The resulting index is split into units of 128 and combined with the inputs.
 *  @param arg0 Base value added to the coarse component.
 *  @param arg1 Base value added to the fractional component.
 *  @param arg2 Value used to select the interval and octave adjustment.
 *  @return Packed result formed by shifting the coarse sum and ORing the fraction.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Scans bits, then searches up to 48 groups of 32 intervals; defaults to 0x600.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u32 gConvertSpuPitchToNote(u16 arg0, u16 arg1, u16 arg2)
{
    register s32 bit __asm__("$10");
    register s32 i __asm__("$10");
    s32 base;
    register s32 one __asm__("$2");
    s32 top, octave, j, index, q, r;
    u16 inverse;
    register u32 power __asm__("$14");
    u32 x, prev, cur, step, lo, hi, lower, upper;
    inverse = ~arg2;
    top = 0;
    for (bit = 15; bit >= 0; bit--) {
        if (((inverse >> bit) & 1) == 0) {
            top = bit;
            break;
        }
    }
    octave = top - 12;
    one = 1;
    __asm__("" : "+r"(one));
    power = one << top;
    x = 0x1000;
    for (i = 0; i < 48; i++) {
        prev = power * x;
        x *= 4155;
        x >>= 12;
        cur = power * x;
        step = (cur - prev) >> 5;
        j = 0;
        __asm__("" : "+r"(j));
        base = i * 32;
        lo = 0;
        hi = step;
        do {
            lower = (prev + lo) >> 12;
            upper = (prev + hi) >> 12;
            if (lower > arg2) goto next;
            if (arg2 < upper) {
                index = base + j;
                goto done;
            }
next:
            hi += step;
            lo += step;
            j++;
        } while (j < 32);
    }
    index = 0x600;
done:
    __asm__("" : : "r"(i));
    q = index / 128;
    r = index - (q << 7);
    return ((arg0 + q + octave * 12) << 8) | (arg1 + r);
}
