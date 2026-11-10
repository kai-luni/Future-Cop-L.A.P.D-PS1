#include "game/types.h"

extern u32 func_800940C4(u8 *p);

void func_80093954(u8 *obj, u8 *arg)
{
    obj[3] = 2;
    *(u32 *)(obj + 4) = func_800940C4(arg);
    *(u32 *)(obj + 8) = 0;
}
