// Categories: Platform > CD-ROM | Utilities > Comparison
#include "game/types.h"

/**
 * @brief Lexicographically compares two 3-byte CD-ROM CdlLOC position triples (minute, second, sector).
 *
 * Confirmed: compares bytes [0], [1], and [2] of a and b in order, returning 1 when b[i] > a[i], -1 when b[i] < a[i], and 0 when all three bytes match. Called by func_8001CE54 to check whether D_8009F614 matches the current disc location D_8009F61C.
 *
 * @param a First 3-byte unsigned coordinate array (CdlLOC minute/second/sector).
 * @param b Second 3-byte unsigned coordinate array (CdlLOC minute/second/sector).
 * @return 1 if b > a, -1 if b < a, or 0 if equal across bytes 0..2.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Six sequential unsigned byte comparisons with early returns (1 or -1), falling through to 0.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8001CE54 (0x8001CFBC in asm/us/main.s, comparing D_8009F61C and D_8009F614) / Likely: none / Unknown: none.
 */
s32 func_8001C3F8(u8 *a, u8 *b)
{
    if (b[0] > a[0]) {
        return 1;
    }
    if (b[0] < a[0]) {
        return -1;
    }
    if (b[1] > a[1]) {
        return 1;
    }
    if (b[1] < a[1]) {
        return -1;
    }
    if (b[2] > a[2]) {
        return 1;
    }
    if (b[2] < a[2]) {
        return -1;
    }
    return 0;
}
