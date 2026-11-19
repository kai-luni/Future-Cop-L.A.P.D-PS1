// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

u32 gComputeU32Reciprocal(u32 divisor) __asm__("func_80018818");

/**
 * @brief Computes the unsigned quotient of 0xFFFFFFFF divided by a supplied
 * word.
 *
 * @param divisor Unsigned divisor.
 * @return floor(0xFFFFFFFF / divisor), or 0xFFFFFFFF for zero.
 *
 * @par Confirmed mechanics
 * Runs 32 shifts of the quotient and partial remainder, beginning with
 * remainder 2. Subtracts the divisor and sets the next quotient bit when the
 * previous shift carried or the divisor is strictly below the remainder.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitReciprocalTable.
 *
 * @par Role and path
 * Engine setup uses the quotient for fixed-point table construction.
 *
 * @par Unknown
 * The table consumer establishes the scale of the quotient.
 */
u32 gComputeU32Reciprocal(u32 divisor)
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
