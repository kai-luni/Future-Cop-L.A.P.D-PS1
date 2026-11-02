#include "game/types.h"

s32 func_8001E304(s32 *ptr)
{
    s32 size = -1;

    if (ptr != 0) {
        size = ptr[-1];
    }

    return size;
}
