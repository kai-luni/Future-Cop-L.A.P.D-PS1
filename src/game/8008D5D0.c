#include "game/types.h"

u8 *func_8008D5D0(u8 *dest, u8 *src, s32 count)
{
    u8 *d;

    d = dest;
    if (d >= src) {
        while (count-- > 0) {
            d[count] = src[count];
        }
    } else {
        while (count-- > 0) {
            *d++ = *src++;
        }
    }
    return d;
}
