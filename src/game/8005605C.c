// Categories: Graphics > Camera > Smoothing | Utilities > Mathematics
#include "game/types.h"

/**
 * @brief Smooths a 12-bit angle toward a target over a number of steps,
 *        taking the shortest way around the circle.
 *
 * @param current Current angle (12-bit).
 * @param target Target angle (12-bit).
 * @param steps Number of easing iterations.
 * @return The smoothed angle (signed-folded, not re-masked).
 *
 * @par Confirmed
 * Both angles fold to signed (-0x800..0x7FF); when |difference| >= 0x800
 * the current angle shifts by +/-0x1000 so the interpolation crosses the
 * wrap. Each step computes current = (current * 31 + target) / 32 (a 1/32
 * exponential ease). When the ease stalls (integer rounding) a linear
 * fallback moves up to |difference|, capped by steps.
 *
 * @par Likely role
 * The yaw/pitch smoothing of the chase camera func_800569EC: the 31/32
 * factor gives a ~32-frame time constant, the stall guard guarantees
 * convergence for small deltas.
 *
 * @par Callers
 * func_800569EC.
 */
s32 func_8005605C(s32 current, s32 target, s32 steps)
{
    s32 difference;
    s32 sign;
    s32 start;
    s32 i;

    current &= 0xFFF;
    if (current >= 0x800) {
        current -= 0x1000;
    }
    target &= 0xFFF;
    if (target >= 0x800) {
        target -= 0x1000;
    }
    difference = target - current;
    sign = difference >> 31;
    if (((sign + difference) ^ sign) >= 0x800) {
        if (difference > 0) {
            current += 0x1000;
        } else {
            current -= 0x1000;
        }
    }
    start = current;
    for (i = steps; i > 0; i--) {
        current = (current * 31 + target) / 32;
    }
    if (current == start && current != target) {
        if (difference < 0) {
            if (-difference < steps) {
                steps = -difference;
            }
            current -= steps;
        } else {
            if (difference < steps) {
                steps = difference;
            }
            current += steps;
        }
    }
    return current;
}
