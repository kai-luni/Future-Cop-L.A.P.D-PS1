#include "game/types.h"

extern s32 D_800A2950[4];

s32 func_8006943C(s32 *arg0)
{
    s32 lower;
    s32 z;
    s32 x;
    s32 changed;

    lower = D_800A2950[0] + 0x800;
    z = arg0[2];
    x = arg0[0];
    changed = 0;

    if (z < lower) {
        arg0[2] = lower;
        changed = 1;
    } else {
        lower = D_800A2950[1] - 0x800;
        if (lower < z) {
            arg0[2] = lower;
            changed = 1;
        }
    }

    lower = D_800A2950[2] + 0x800;
    if (x < lower) {
        arg0[0] = lower;
        changed |= 2;
    } else {
        lower = D_800A2950[3] - 0x800;
        if (lower < x) {
            arg0[0] = lower;
            changed |= 2;
        }
    }

    return changed;
}
