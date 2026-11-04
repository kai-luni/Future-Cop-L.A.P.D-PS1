#include "game/types.h"

void func_8007AA9C(u8 *dst, u8 *src, s32 len, u8 *table)
{
    s32 i;
    s32 flag = 0;

    dst[len] = 0;
    for (i = len - 1; i >= 0; i--) {
        u8 c = table[src[i]];
        if (c != 0x20) {
            flag = 1;
        }
        if (!flag) {
            c = 0;
        }
        dst[i] = c;
    }
    if (dst[0] == 0) {
        dst[0] = 0x20;
    }
}
