// Categories: Physics > Collision | Gameplay > Movement
#include "game/types.h"

struct Bound {
    s16 margin;
    u16 y;
};

extern s32 func_800640D8(u16 *in, s16 *out);
extern s32 func_80063FF8(s16 *point, s32 margin);

/**
 * @brief Builds a four-direction collision mask around a position.
 *
 * @param base Geometry block (half-word array).
 * @param step Offset added per direction test.
 * @param bounds Four margin records; the margins at [1]..[4] are used, shifted
 *        left by 4.
 * @return A bit mask: bit 1 = +step, bit 2, bit 4, bit 3 - one bit per direction.
 *
 * @par Confirmed
 * Each direction runs func_800640D8(in, out) (a ray/height query) and, when it
 * finds no hit, ORs `func_80063FF8(out, margin << 4) << n` into the result with
 * n = 1, 2, 4 and 3 respectively.
 *
 * @par Likely role
 * The "can I move this way" probe of the entity movement: four directions, each
 * tested with the object's own margin, and the result tells the mover which sides
 * are free.
 */
s32 func_8006411C(u16 *base, s32 step, struct Bound *bounds)
{
    s32 result;
    s32 half;
    u16 in[3];
    s16 out[3];

    result = 0;
    half = step >> 1;

    in[0] = base[0] + half;
    in[1] = base[1] + bounds[1].y;
    in[2] = base[2] + half;

    if (func_800640D8(in, out) == 0) {
        result = func_80063FF8(out, bounds[1].margin << 4) << 1;
    }

    in[1] = base[1] + bounds[2].y;
    in[2] += step;

    if (func_800640D8(in, out) == 0) {
        result |= func_80063FF8(out, bounds[2].margin << 4) << 2;
    }

    in[0] += step;
    in[1] = base[1] + bounds[4].y;

    if (func_800640D8(in, out) == 0) {
        result |= func_80063FF8(out, bounds[4].margin << 4) << 4;
    }

    in[1] = base[1] + bounds[3].y;
    in[2] -= step;

    if (func_800640D8(in, out) == 0) {
        result |= func_80063FF8(out, bounds[3].margin << 4) << 3;
    }

    return result;
}
