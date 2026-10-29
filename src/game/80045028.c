#include "game/types.h"

extern s32 D_800A1800;

s32 func_80045028(s32 mode)
{
    s32 state = D_800A1800;

    if (state != -1 && mode == state) {
        return 1;
    }
    return 0;
}
