// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

/**
 * @brief Computes the 24-bit integer square root floor(sqrt(value * 2^16)) = floor(256 * sqrt(value)).
 *
 * Performs a 24-iteration restoring binary square-root loop (16 iterations consuming the 32 bits
 * of value two bits at a time, plus 8 fractional zero-bit-pair iterations).
 *
 * @param value Unsigned 32-bit input value.
 * @return Fixed-point root floor(256 * sqrt(value)) in [0, 0xFFFFFF].
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single 24-iteration do-while loop (i = 0x17 down to 0) testing remainder >= (root << 1) + 1.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80019828 (0x80019860, converts func_80018818 reciprocals into roots for the D_8009F5B0 table) / Likely: none / Unknown: none.
 */
u32 func_800187C8(u32 value)
{
    u32 root;
    u32 remainder;
    u32 trial;
    s32 i;

    root = 0;
    remainder = root;
    i = 0x17;
    do {
        remainder = (remainder << 2) | (value >> 30);
        root <<= 1;
        trial = (root << 1) + 1;
        value <<= 2;
        if (remainder >= trial) {
            remainder -= trial;
            root++;
        }
    } while (--i != -1);
    return root;
}
