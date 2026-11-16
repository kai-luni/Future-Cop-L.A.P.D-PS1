// Categories: Game Engine > State Inspection
#include "game/types.h"

extern u8 *D_8009F4D8;

/**
 *  @brief Counts nonzero words in a four-entry state region.
 *
 *  Examines four consecutive 32-bit values beginning at byte offset
 *  0x7B2C from the base pointed to by D_8009F4D8.
 *  @return Number of nonzero entries, from zero through four.
 *  @par Globals
 *  D_8009F4D8 supplies the base address of the inspected region.
 *  @par Control flow
 *  A four-iteration loop increments the count for each nonzero word.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80086FA8(void)
{
    s32 count = 0;
    u32 i = count;
    u8 *ptr = D_8009F4D8;

    do {
        if (*(u32 *)(ptr + 0x7B2C) != 0) {
            count++;
        }
        i++;
        ptr += 4;
    } while (i < 4);

    return count;
}
