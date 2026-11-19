// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

/**
 * @brief Computes the 32-bit unsigned reciprocal floor((2^32 - 1) / divisor).
 *
 * Runs a 32-iteration binary restoring division loop starting with remainder = 2 and quotient = 0,
 * tracking overflow in carry (bit 31 of remainder) and returning 0xFFFFFFFF when divisor is 0.
 *
 * @param divisor Unsigned 32-bit divisor (0 yields 0xFFFFFFFF).
 * @return Quotient floor(0xFFFFFFFF / divisor) for divisor >= 1, or 0xFFFFFFFF for 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Fixed 32-iteration descending for-loop testing carry != 0 || divisor < remainder each bit.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80019828 (0x80019858, computes reciprocal input 0..0x3FF before gSqrtU32Q8 builds D_8009F5B0) / Likely: none / Unknown: none.
 */
u32 func_80018818(u32 divisor)
{
    u32 remainder;
    u32 quotient;
    u32 carry;
    s32 i;

    remainder = 2;
    quotient = 0;
    carry = quotient;
    for (i = 32; i > 0; i--) {
        quotient <<= 1;
        if (carry != 0 || divisor < remainder) {
            remainder -= divisor;
            quotient |= 1;
        }
        carry = remainder & 0x80000000;
        remainder <<= 1;
    }
    return quotient;
}
