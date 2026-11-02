#include "game/types.h"

extern s32 D_80010028;
extern s32 D_8001002C;

extern s32 *func_8008CA90(s32 size);

s32 *func_8001E240(s32 size)
{
    s32 *block;
    s32 next;

    if (size == 0) {
        return 0;
    }

    block = func_8008CA90(size + 4);

    next = D_80010028 + 4;
    next += size;
    D_80010028 = next;
    if (next < D_8001002C) {
        next = D_8001002C;
    }
    D_8001002C = next;

    if (block == 0) {
        return 0;
    }

    *block = size;
    return block + 1;
}
