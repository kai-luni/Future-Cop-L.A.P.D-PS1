#include "game/types.h"

u8 *func_8001E318(u8 *destination, u8 *source, u32 count)
{
    if (((u32)destination & 3) == 0 &&
        ((u32)source & 3) == 0 &&
        (count & 3) == 0) {
        u32 *d = (u32 *)destination;
        u32 *s = (u32 *)source;
        u32 n = count >> 2;
        if (n >= 8) {
            do {
                u32 a, b, c, e;
                a = s[0]; b = s[1]; c = s[2]; e = s[3];
                d[0] = a; d[1] = b; d[2] = c; d[3] = e;
                a = s[4]; b = s[5]; c = s[6]; e = s[7];
                d[4] = a; d[5] = b; d[6] = c; d[7] = e;
                d += 8;
                s += 8;
                n -= 8;
            } while (n >= 8);
        }
        while (n != 0) {
            *d++ = *s++;
            n--;
        }
        return destination;
    }
    if (((u32)destination & 1) == 0 &&
        ((u32)source & 1) == 0 &&
        (count & 1) == 0) {
        u16 *d = (u16 *)destination;
        u16 *s = (u16 *)source;
        u32 n = count >> 1;
        while (n != 0) {
            *d++ = *s++;
            n--;
        }
        return destination;
    }
    {
        u8 *d = destination;
        while (count != 0) {
            *d++ = *source++;
            count--;
        }
    }
    return destination;
}
