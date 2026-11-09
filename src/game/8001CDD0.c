#include "game/types.h"

extern void *func_80017EBC(s32 size, s32 tag);

void *func_8001CDD0(s32 size)
{
    return func_80017EBC(size, 2);
}
