// Categories: Utilities > Mathematics > Vector | Physics > Collision
#include "game/types.h"

extern s32 func_8001A074(s32 *vector);

/**
 * @brief Returns the cheap two-component length of the XZ offset between two
 *        three-word vectors.
 *
 * Builds a two-word difference with the axes swapped - index 2 first, then
 * index 0 - and hands it to the length estimate func_8001A074, so the y
 * component of both inputs is ignored and the result is a ground-plane range.
 *
 * @param a First vector, three words (x, y, z); only +0x0 and +0x8 are read.
 * @param b Second vector, same layout.
 * @return An approximation of the length of (a.z - b.z, a.x - b.x).
 *
 * @par Globals
 * None; the function works only on its arguments.
 *
 * @par Control flow
 * Two subtractions into an 8-byte stack local, then one call. No branches.
 *
 * @par Calls
 * func_8001A074 - absolute-value, sort and 1/128 + 1/64 correction length
 * estimate over the two prepared components.
 *
 * @par Side effects
 * None observed: the stack local is the only memory it writes.
 *
 * @par Callers
 * Confirmed: func_80035B7C (mover update), func_800513D0.
 */
s32 func_8001A13C(s32 *a, s32 *b)
{
    s32 delta[2];

    delta[0] = a[2] - b[2];
    delta[1] = a[0] - b[0];
    return func_8001A074(delta);
}
