// Categories: Game Engine > Address Lookup
#include "../../include/game/types.h"

extern u8 gCompareAddress[] __asm__("D_800A3C20");
int gGetPadPortCode(void *address) __asm__("func_8008C3B4");


/**
 *  @brief Maps either of two exact addresses to a small code.
 *
 *  Compares the supplied address against D_800A3C20 and the address
 *  0xF0 bytes after it, returning a distinct code for each match.
 *  @param address Address to compare against the two candidates.
 *  @return 0x10 for the first address, 0x20 for the second, or 0xFF for neither.
 *  @par Globals
 *  D_800A3C20 supplies the first candidate address.
 *  @par Control flow
 *  Loops over two candidates and returns immediately on a match.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Compares address against gCompareAddress (D_800A3C20) and gCompareAddress +
 * 0xF0 in a two-iteration loop, returning 0x10 for the first port struct,
 * 0x20 for the second, or 0xFF if neither matches. Bound to D_8009C738 by
 * func_8008BDB0.
 */
int gGetPadPortCode(void *address)
{
    int index;
    int result;
    u8 *current;

    index = 0;
    result = 0x10;
    current = gCompareAddress;

    for (; index < 2; index++) {
        if (address == current) {
            return result;
        }
        result += 0x10;
        current += 0xF0;
    }
    return 0xFF;
}
