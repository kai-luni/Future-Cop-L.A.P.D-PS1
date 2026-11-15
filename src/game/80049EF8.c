// Categories: Utilities > Mathematics | Graphics > Clipping
#include "game/types.h"

/**
 * @brief Side test for three packed 2D points: sign of the cross product
 *        of (first - origin) and (second - origin).
 *
 * @param origin Packed point, x in the low halfword, y in the high.
 * @param first Packed point.
 * @param second Packed point.
 * @return 1 when the cross product is >= 0, else 0.
 *
 * @par Confirmed
 * ((s16)first - (s16)origin) * ((second >> 16) - (origin >> 16)) -
 * ((s16)second - (s16)origin) * ((first >> 16) - (origin >> 16)) >= 0.
 * The low halfwords are sign-extended, the high halves used as-is.
 *
 * @par Likely role
 * Half-plane test for convex clipping or winding checks in the map-cell
 * sprite path.
 *
 * @par Callers
 * func_8004A048.
 */
s32 func_80049EF8(s32 origin, s32 first, s32 second)
{
    return ((s16)first - (s16)origin) * ((second >> 16) - (origin >> 16)) -
           ((s16)second - (s16)origin) * ((first >> 16) - (origin >> 16)) >= 0;
}
