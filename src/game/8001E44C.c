// Categories: Utilities > Memory
#include "game/types.h"
u8 *gMoveMemory(u8 *destination, u8 *source, s32 count) __asm__("func_8001E44C");


/**
 * @brief memmove(): copies bytes with overlap handling.
 *
 * @param destination Target address.
 * @param source Source address.
 * @param count Number of bytes.
 * @return The unchanged destination pointer.
 *
 * @par Confirmed flow
 * If source < destination < source + count (the regions overlap forwards) the
 * copy runs backwards from the end, byte by byte; otherwise it runs forward.
 *
 * @par Callers
 * func_800181D4, when the free-range table has to be compacted in place - that
 * is the only place where the direction matters.
 *
 * @par Role and path
 * Copies count bytes from source to destination, copying backward from the
 * end when source < destination < source + count and forward otherwise.
 * Touches no globals and is called by gFreeBlock when compacting overlapping
 * free-range table entries in place.
 */
u8 *gMoveMemory(u8 *destination, u8 *source, s32 count)
{
    u8 *to;
    u8 *from;

    to = destination;
    if (source < destination && destination < source + count) {
        to = destination + count;
        from = source + count;
        while (count != 0) {
            *--to = *--from;
            count--;
        }
        return destination;
    }
    while (count != 0) {
        *to++ = *source++;
        count--;
    }
    return destination;
}
