#include "game/types.h"

extern s32 D_8009F648;

extern s32 func_800451E0(void);
extern void func_8002283C(s32 channel, s32 left, s32 right);

void func_80024730(s32 volume)
{
    if (D_8009F648 != 0) {
        if (func_800451E0() != 0) {
            func_8002283C(0, volume, 0);
            func_8002283C(1, volume, 0x7F);
        } else {
            func_8002283C(0, volume, 0x40);
            func_8002283C(1, volume, 0x40);
        }
    }
}
