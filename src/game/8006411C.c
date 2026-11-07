#include "game/types.h"

struct Bound {
    s16 margin;
    u16 y;
};

extern s32 func_800640D8(u16 *in, s16 *out);
extern s32 func_80063FF8(s16 *point, s32 margin);

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
