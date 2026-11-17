// Categories: Utilities > Mathematics > Trigonometry
#include "game/types.h"

extern s16 gRatioTable[] __asm__("D_8009E0A0");

/**
 * @brief Two-argument arctangent over a 1024-entry table, 12-bit angle result.
 *
 * @param x, y Signed components.
 * @return The angle in 12-bit units (0..0xFFF, signed around the axes),
 *         0 for the origin.
 *
 * @par Confirmed
 * Both signs are folded out (negateX/negateY); the smaller component is
 * divided by the larger >> 10 (or shifted << 10 first when the large value
 * would overflow the division), the ratio indexes D_8009E0A0. x < y reads
 * the table directly, else 0x400 - table. The y sign mirrors via
 * 0x800 - result, the x sign negates.
 *
 * @par Likely role
 * The heading/bearing source of the game: the radar func_8005A934, the
 * mover integrator func_8005027C and the AI handlers compute their target
 * angles with it; the 12-bit result matches the 0xFFF angle masks used
 * throughout the entity code.
 *
 * @par Callers
 * func_8002AD58, func_8002B6C8, func_8002CC30, func_8002E630, func_80038A00,
 * func_8004F764, func_8005027C, func_80050E0C, func_80057E40, func_800592D0,
 * func_8005A934, func_8005E624.
 */
s32 func_80095C30(s32 x, s32 y)
{
    s32 result;
    s32 negateY;
    s32 negateX;
    s32 index;

    negateY = 0;
    negateX = 0;
    if (y < 0) {
        negateY = 1;
        y = -y;
    }
    if (x < 0) {
        negateX = 1;
        x = -x;
    }
    if (y == 0 && x == 0) {
        return 0;
    }
    if (x < y) {
        if (x & 0x7FE00000) {
            index = x / (y >> 10);
        } else {
            index = (x << 10) / y;
        }
        result = gRatioTable[index];
    } else {
        if (y & 0x7FE00000) {
            index = y / (x >> 10);
        } else {
            index = (y << 10) / x;
        }
        result = 0x400 - gRatioTable[index];
    }
    if (negateY) {
        result = 0x800 - result;
    }
    if (negateX) {
        result = -result;
    }
    return result;
}
