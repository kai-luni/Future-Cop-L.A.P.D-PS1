// Categories: Game Engine > State Inspection
#include "game/types.h"

extern u8 *gMovieAudioState __asm__("D_8009F4D8");

/**
 *  @brief Counts nonzero words in a four-entry state region.
 *
 *  Examines four consecutive 32-bit values beginning at byte offset
 *  0x7B2C from the base pointed to by gMovieAudioState.
 *  @return Number of nonzero entries, from zero through four.
 *  @par Globals
 *  gMovieAudioState supplies the base address of the inspected region.
 *  @par Control flow
 *  A four-iteration loop increments the count for each nonzero word.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 gMovieAudioBusyCount(void) __asm__("func_80086FA8");

s32 gMovieAudioBusyCount(void)
{
    s32 count = 0;
    u32 i = count;
    u8 *ptr = gMovieAudioState;

    do {
        if (*(u32 *)(ptr + 0x7B2C) != 0) {
            count++;
        }
        i++;
        ptr += 4;
    } while (i < 4);

    return count;
}
