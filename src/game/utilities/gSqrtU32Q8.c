// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

u32 gSqrtU32Q8(u32 value) __asm__("func_800187C8");

/**
 * @brief Computes an unsigned square root with eight fractional bits.
 *
 * @param value Unsigned 32-bit radicand.
 * @return floor(256 * sqrt(value)).
 *
 * @par Confirmed mechanics
 * Runs 24 iterations of restoring square root. Shifts two input bits into the
 * remainder, doubles the partial root, and tests trial divisor 2 times the
 * partial root plus 1 before subtracting and setting the next bit.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitReciprocalTable.
 *
 * @par Role and path
 * The engine setup invokes it while building fixed-point lookup values.
 *
 * @par Unknown
 * The consumer determines the units of the radicand.
 */
u32 gSqrtU32Q8(u32 value)
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
