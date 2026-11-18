// Categories: Graphics > Model Renderer > Culling
#include "game/types.h"
s32 gEmitFrontFacingTriangle(s32 origin, s32 first, s32 second, s32 *out) __asm__("func_80015AF4");


/**
 * @brief Backface/winding test for a triangle in packed 16.16 coordinates;
 *        emits the vertex triple when it faces the viewer.
 *
 * @param origin, first, second Packed coordinates (low 16 bits one axis,
 *        high 16 bits the other).
 * @param out Receives the three values when the test passes.
 * @return 1 when the triangle is culled (negative cross product), else 0.
 *
 * @par Confirmed
 * The cross product (first-origin) x (second-origin) is computed from the
 * split halves; a negative result returns 1 without writing out, otherwise
 * out[0..2] = origin/first/second and 0 is returned.
 *
 * @par Likely role
 * The per-face orientation gate of the model part setup func_80015B68:
 * the packed format matches the GTE vertex words, the sign convention
 * selects the visible winding.
 *
 * @par Callers
 * func_80015B68.
 *
 * @par Environment path
 * Backface/winding test for a triangle in packed 16.16 coordinates; emits the
 * vertex triple only when it faces the viewer.
 */
s32 gEmitFrontFacingTriangle(s32 origin, s32 first, s32 second, s32 *out)
{
    if (((s16)first - (s16)origin) * ((second >> 16) - (origin >> 16)) -
        ((s16)second - (s16)origin) * ((first >> 16) - (origin >> 16)) < 0) {
        return 1;
    }
    out[0] = origin;
    out[1] = first;
    out[2] = second;
    return 0;
}
