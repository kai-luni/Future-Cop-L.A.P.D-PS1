// Categories: Utilities > Bit Manipulation | Graphics > Textures
#include "game/types.h"
s32 gCountTrailingZeros(u32 value) __asm__("func_800195F0");


/**
 * @brief Counts trailing zero bits in a 32-bit unsigned value (lowest set bit index).
 *
 * Confirmed: starts with count = 32 and halves the search window via left shifts (<< 16, << 8, << 4, << 2, << 1), subtracting 16, 8, 4, 2, 1 when the shifted bits remain non-zero, then subtracts 1 if the final bit is non-zero. Returns 0..31 for bit index, or 32 for 0.
 *
 * @param value Unsigned 32-bit integer to inspect.
 * @return Index of least-significant set bit (0..31), or 32 when value == 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line binary-search sequence of six conditional branches on left-shifted value; no loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8005A2BC (0x8005A33C in asm/us/main.s, converts byte +3 of sprite/tile entry into shift/exponent) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Pure bit utility that feeds the texture descriptor path, not a draw
 * routine: returns 0..31 as the lowest set bit position, i.e. log2 for a
 * power-of-two input, or 32 for 0. func_8005A2BC applies it to byte 3 of each
 * atlas block when building its descriptor table.
 */
s32 gCountTrailingZeros(u32 value)
{
    s32 count;

    count = 32;
    if ((value << 16) != 0) {
        value <<= 16;
        count = 16;
    }
    if ((value << 8) != 0) {
        value <<= 8;
        count -= 8;
    }
    if ((value << 4) != 0) {
        value <<= 4;
        count -= 4;
    }
    if ((value << 2) != 0) {
        value <<= 2;
        count -= 2;
    }
    if ((value << 1) != 0) {
        value <<= 1;
        count -= 1;
    }
    if (value != 0) {
        count -= 1;
    }
    return count;
}
