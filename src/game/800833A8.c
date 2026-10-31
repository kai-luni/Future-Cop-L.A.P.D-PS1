#include "game/types.h"

extern s32 D_8009C1CC[5];
extern s32 D_8009F3D8;
extern s32 D_8009F3E0;

extern s32 func_8008CCA0(s32 arg0, s32 arg1);

s32 func_800833A8(s32 arg0)
{
    u32 i;

    for (i = 0; i < 5; i++) {
        if (func_8008CCA0(arg0, D_8009C1CC[i]) == 0) {
            break;
        }
    }

    if (i == 5) {
        return D_8009F3D8;
    }
    return D_8009F3E0;
}
