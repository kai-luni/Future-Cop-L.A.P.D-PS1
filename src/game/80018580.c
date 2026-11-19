// Categories: Utilities > Mathematics > Trigonometry

int gApproximateAngle256(int x, int y) __asm__("func_80018580");

/**
 * @brief Approximates the signed angle of two components on a 256-unit circle.
 *
 * @param x First signed component, the zero-angle axis.
 * @param y Second signed component.
 * @return Signed angle in 256 units per turn, or zero at the origin.
 *
 * @par Confirmed mechanics
 * Handles the zero vector first. The two signs and the absolute-magnitude
 * comparison select one of eight jump-table cases. Each case divides the
 * smaller magnitude shifted left by five by the larger magnitude and applies a
 * signed offset from 0, 64, or 128.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_8005AB8C.
 *
 * @par Role and path
 * The heading calculation in func_8005AB8C uses this coarse angle.
 *
 * @par Unknown
 * The arithmetic does not identify the components with world axes.
 */
int gApproximateAngle256(int x, int y)
{
    unsigned char flags;
    int ax, ay;
    if (x == 0 && y == 0) return 0;
    flags = (unsigned int)x >> 31;
    ax = ((x >> 31) + x) ^ (x >> 31);
    ay = ((y >> 31) + y) ^ (y >> 31);
    if (y < 0) flags |= 2;
    if (ax < ay) flags |= 4;
    switch (flags) {
    case 7: return -64 - ((ax << 5) / ay);
    case 6: return ((ax << 5) / ay) - 64;
    case 5: return ((ax << 5) / ay) + 64;
    case 4: return 64 - ((ax << 5) / ay);
    case 3: return ((ay << 5) / ax) - 128;
    case 2: return -((ay << 5) / ax);
    case 1: return 128 - ((ay << 5) / ax);
    case 0: return (ay << 5) / ax;
    }
    return 0;
}
