// Categories: Utilities > Mathematics > Trigonometry
/**
 * @brief Eight-octant arctangent with a 256-unit full circle (32 units per
 *        quadrant).
 *
 * @param x, y Signed components.
 * @return The angle in -128..128 units, 0 for the origin.
 *
 * @par Confirmed
 * The signs and the |x| < |y| comparison build a 3-bit case selector;
 * each case divides the smaller component << 5 by the larger and offsets
 * by 0/64/128 with the matching sign. Pure arithmetic, no tables.
 *
 * @par Likely role
 * The coarse heading solver of the radar sweep func_8005AB8C: the 256-unit
 * circle matches the byte-wide angle fields, unlike the 12-bit
 * func_80095C30 used by the entity code.
 *
 * @par Callers
 * func_8005AB8C.
 */
int func_80018580(int x, int y)
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
