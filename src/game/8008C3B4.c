// Categories: Game Engine > Address Lookup
#include "../../include/game/types.h"

extern u8 D_800A3C20[];

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
 */
int func_8008C3B4(void *address)
{
    int index;
    int result;
    u8 *current;

    index = 0;
    result = 0x10;
    current = D_800A3C20;

    for (; index < 2; index++) {
        if (address == current) {
            return result;
        }
        result += 0x10;
        current += 0xF0;
    }
    return 0xFF;
}
