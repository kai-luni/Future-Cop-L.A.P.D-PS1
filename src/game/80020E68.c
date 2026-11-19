// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

extern void gParseCardDirectory(void) __asm__("func_800207CC");
extern s32 gReadCardSlotState(u32 slot) __asm__("func_80020DF8");

u32 gFindFreeCardBlockRun(u32 size) __asm__("func_80020E68");

/**
 * @brief Finds a contiguous run of card slots marked free that can hold a
 * requested byte count.
 *
 * @param size Requested file size in bytes.
 * @return Starting slot of the first qualifying run, or zero if none is found.
 *
 * @par Confirmed mechanics
 * Refreshes the directory and rounds the requested size up to 0x2000-byte
 * blocks. For each candidate, advances through consecutive slots while
 * gReadCardSlotState returns 2. Later candidates are limited by 16 minus the
 * block count.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80020FDC, func_8007FB04.
 *
 * @par Role and path
 * Card save creation uses the returned start index to stage allocation.
 *
 * @par Unknown
 * A result of zero is shared by the first candidate and the miss path.
 */
u32 gFindFreeCardBlockRun(u32 size)
{
    register u32 k asm("s0") = size;
    register u32 j asm("s1");
    register u32 i asm("s2");
    register u32 limit asm("s3");
    register u32 result asm("s4") = 0;
    register u32 two asm("s5");
    register u32 bound asm("s6");
    register u32 tmp asm("v0");

    asm("" : "+r"(k), "+r"(result));
    gParseCardDirectory();
    i = result;
    two = 2;
    k = (k + 0x1FFF) >> 13;
    tmp = 16;
    asm("" : "+r"(tmp));
    bound = tmp - k;
    j = k;
    do {
        k = i;
        if (i < j) {
            limit = j;
            do {
                if (gReadCardSlotState(k) != two)
                    break;
                k++;
            } while (k < limit);
        }
        if (k == j) {
            result = i;
            break;
        }
        i++;
        j++;
    } while (i <= bound);
    return result;
}
