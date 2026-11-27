// Categories: Utilities > Memory
#include "game/types.h"
u8 *gCopyMemory(u8 *destination, u8 *source, u32 count) __asm__("func_8001E318");


/**
 * @brief memcpy/memmove: copies @c count bytes with the widest safe access.
 *
 * @param destination Target address.
 * @param source Source address.
 * @param count Number of bytes.
 * @return The unchanged destination pointer.
 *
 * @par Confirmed structure
 * Three paths chosen by alignment: 4-byte aligned source, destination and
 * length use an unrolled eight-word loop; 2-byte alignment uses a half-word
 * loop; anything else a byte loop. There is no overlap handling - the one
 * caller that has to move backwards (the free-range merge) uses
 * func_8001E44C instead.
 *
 * @par Callers
 * func_800181D4 and func_80017EBC, for the free-range table maintenance.
 *
 * @par Role and path
 * Copies count bytes from source to destination using an unrolled 8-word loop
 * for 4-byte aligned buffers, a 16-bit loop for 2-byte aligned buffers, or a
 * byte loop otherwise, without overlap handling. Touches no globals and is
 * called by gAllocMemory and gFreeBlock for free-range table maintenance.
 */
u8 *gCopyMemory(u8 *destination, u8 *source, u32 count)
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
