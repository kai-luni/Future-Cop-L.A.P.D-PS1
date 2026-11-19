// Categories: Utilities > Mathematics > Vector Length | Graphics > Culling
#include "game/types.h"

extern s32 func_8001A074(s32 *vector);
s32 gVectorLength3D(s32 *vector) __asm__("func_8001A100");


/**
 * @brief Length of a 3D vector, computed in two planar steps.
 *
 * @param vector Pointer to three 32-bit components (x, y, z).
 * @return An integer approximation of sqrt(x^2 + y^2 + z^2).
 *
 * @par Confirmed flow
 * planar[0] = func_8001A074(vector) - the length of the xy pair -,
 * planar[1] = vector[2], and the result is func_8001A074(planar), i.e. the same
 * routine applied to (xy-length, z). func_8001A074 itself is an integer
 * sqrt(x^2 + y^2) built from absolute values, a swap, one shift-add correction
 * and two final shifts - no division, no floating point.
 *
 * @par Why two steps
 * The helper only handles two components, so a 3D length needs the chained
 * call; the error stays inside the tolerance the renderer uses.
 *
 * @par Callers
 * 22 sites - every renderer and effect walker that needs a distance:
 * func_8006DC24, func_80071FA4, func_8003E984, func_8006F310, func_80074104 and
 * the rest of the polygon/particle paths. Likely used for LOD and range tests.
 *
 * @par Environment path
 * Returns an integer approximation of the 3D length of a vector by applying
 * the 2D length helper func_8001A074 to (x, y) and then to (xy-length, z). It
 * has 22 callers, among them gDrawNodeListModels (func_8003CF6C),
 * gDrawObjectPointTiles and gDrawTracerBand (func_8006F310), which use it for
 * distance and range rejects against the view position. It is shared geometry
 * math the camera-distance culling relies on.
 */
s32 gVectorLength3D(s32 *vector)
{
    s32 planar[2];

    planar[0] = func_8001A074(vector);
    planar[1] = vector[2];
    return func_8001A074(planar);
}
