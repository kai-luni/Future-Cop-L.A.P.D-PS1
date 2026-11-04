#include "game/types.h"

extern s32 func_8008CCC0(u8 *str);

void func_8007A9D0(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen)
{
    s32 i;
    s32 j;
    s32 limit;
    s32 c;

    limit = func_8008CCC0(src);
    for (i = 0; i < len; i++) {
        if (i < limit) {
            c = src[i];
        } else {
            c = 0x20;
        }
        for (j = 0; j < tableLen; j++) {
            if (c == table[j]) {
                break;
            }
        }
        if (j == tableLen) {
            j = 0;
        }
        dst[i] = j;
    }
}
