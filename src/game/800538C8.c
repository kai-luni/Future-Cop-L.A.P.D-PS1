// Categories: Platform > Controller Input | Input > Analog
#include "game/types.h"

extern s32 func_8001E7B8(s32 *x, s32 *y, s32 controller, s32 arg3);

/**
 * @brief Converts the analog stick of one controller into direction bits and magnitudes.
 *
 * Nothing happens while the top nibble of *flags is already set, while the
 * caller's 0xC0 byte is non-zero, or while 0x310 is set. func_8001E7B8 then
 * returns the raw x/y pair; both axes are centred by subtracting 0x80 and the
 * values inside +/-0x27 are forced to zero. Beyond 0xE1 the axis saturates at
 * 0x100, below -0xE0 at -0x100, and each non-zero axis sets its direction bit
 * (0x20000000/0x10000000 for x, 0x80000000/0x40000000 for y). The magnitudes
 * are written as absolute values, so the caller gets direction and strength
 * from one call.
 *
 * @param context Caller state; controller index at 0xC1, lock bytes at 0xC0/0x310.
 * @param flags Direction bits are OR-ed into this word.
 * @param verticalMagnitude Absolute y magnitude.
 * @param horizontalMagnitude Absolute x magnitude.
 * @return The raw status from func_8001E7B8, or 0 when no stick was read.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Guard, read, centre, clamp per axis, then the bit assignments.
 *
 * @par Calls
 * func_8001E7B8 - raw analog read.
 *
 * @par Side effects
 * Writes *flags and both magnitudes.
 *
 * @par Callers
 * Confirmed: func_80053A74, func_800549F0.
 */
s32 func_800538C8(u8 *context, u32 *flags, s32 *verticalMagnitude,
                  s32 *horizontalMagnitude)
{
    s32 x;
    s32 y;
    s32 status;
    u32 direction;
    s32 sign;

    if ((*flags & 0xF0000000) == 0) {
        if (*(s8 *)(context + 0xC0) != 0) {
            return 0;
        }
        if (*(s8 *)(context + 0x310) == 0) {
            status = func_8001E7B8(&x, &y, *(s8 *)(context + 0xC1), 0);
            if ((u8)status == 0) {
                return 0;
            }

            direction = 0;
            x -= 0x80;
            y -= 0x80;
            if ((u32)(x + 0x27) < 0x4F) {
                x = 0;
            } else if (x >= 0xE1) {
                x = 0x100;
            } else if (x < -0xE0) {
                x = -0x100;
            }

            if (x > 0) {
                direction |= 0x20000000;
            } else if (x < 0) {
                direction |= 0x10000000;
            }

            if ((u32)(y + 0x27) < 0x4F) {
                y = 0;
            } else if (y >= 0xE1) {
                y = 0x100;
            } else if (y < -0xE0) {
                y = -0x100;
            }

            if (y > 0) {
                direction |= 0x80000000;
            } else if (y < 0) {
                direction |= 0x40000000;
            }

            *flags |= direction;
            sign = y >> 31;
            *verticalMagnitude = (y + sign) ^ sign;
            sign = x >> 31;
            *horizontalMagnitude = (x + sign) ^ sign;
            return (u8)status;
        }
    }

    return 0;
}
