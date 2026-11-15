// Categories: Utilities > Mathematics > Vector Length | Graphics > Culling
#include "game/types.h"

/**
 * @brief Two-component length estimate with a 1/128 + 1/64 correction.
 *
 * @param vector Pointer to two 32-bit components.
 * @return An approximation of sqrt(x^2 + y^2) in the same scale as the input.
 *
 * @par Confirmed
 * Both components are made absolute and sorted (a >= b via XOR swap). The result
 * is `a + (b >> 2) - (b >> 6)`, except when `a / 2 < b`, where the branch builds
 * `(64 + 8 + 2) * sum + a + 32 * b`, shifts right by 7, and returns
 * `a + (b >> 2) - (b >> 6)` with a = that sum and b = a - b swapped first.
 *
 * @par Likely role
 * The cheap distance used for range and marker tests (func_8003E984,
 * func_8005B088, func_80070730); func_80019F54 is the three-component variant.
 */
s32 func_8001A074(s32 *vector)
{
    s32 a;
    s32 b;
    s32 sum;

    a = vector[0];
    if (a < 0) {
        a = -a;
    }
    b = vector[1];
    if (b < 0) {
        b = -b;
    }
    if (a < b) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    sum = a + b;
    if ((a >> 1) < b) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + a + (b << 5);
        b = a - b;
        a = sum >> 7;
    }
    return a + (b >> 2) - (b >> 6);
}
