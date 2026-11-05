#include "game/types.h"

extern u32 func_800940A8(u32 x, u32 y);

void func_80093A0C(u8 *obj, s16 *rect)
{
    obj[3] = 2;
    *(s32 *)(obj + 4) = func_800940A8(*(s16 *)(rect + 0), rect[1]);
    *(s32 *)(obj + 8) = 0;
}
