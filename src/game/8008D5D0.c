// Categories: Utilities > Memory
#include "game/types.h"
u8 *gCopyMemOverlap(u8 *dest, u8 *src, s32 count) __asm__("func_8008D5D0");


/**
 *  @brief Copies bytes in a direction chosen to handle overlap.
 *
 *  Copies backward when the destination is at or above the source and
 *  forward otherwise; nonpositive counts perform no copy.
 *  @param dest Destination buffer.
 *  @param src Source buffer.
 *  @param count Number of bytes to copy.
 *  @return Original destination for backward copying, or the advanced destination for forward copying.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Selects a direction by pointer comparison and loops while count is positive.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the copied bytes to the destination buffer.
 *  @par Callers
 *  Confirmed: func_8008CD3C.
 *
 * @par Role and path
 * Copies count bytes from src to dest, iterating backward when dest >= src
 * and forward when dest < src to handle overlapping buffers. Called by
 * func_8008CD3C; returns dest for backward copies or dest + count for forward
 * copies.
 */
u8 *gCopyMemOverlap(u8 *dest, u8 *src, s32 count)
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
