// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 gFreeRangeCursor __asm__("D_8009F5AC");
extern s32 gStateEntryTable[] __asm__("D_800A04D0");

s32 gAdvanceSecondaryPrng(void) __asm__("func_800196C4");

/**
 * @brief Advances the secondary lagged-additive random sequence and returns
 * its updated word.
 * @return The updated 32-bit state word.
 *
 * @par Confirmed mechanics
 * Decrements the cursor, wrapping a negative result to 19. Forms the second
 * index from the old cursor plus 2 or old cursor minus 18. Adds both words
 * with ADDU, stores the sum in the decremented slot, and returns that word.
 *
 * @par Globals
 * Reads and writes gFreeRangeCursor and gStateEntryTable. The existing name
 * gFreeRangeCursor denotes the cursor of this PRNG stream here.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_8003D0F0,
 * gHandleLightObjectMsg, gDrawCellWindowModels.
 *
 * @par Role and path
 * gSeedPrng initializes this second stream, which is used by a separate set of
 * entity routines.
 *
 * @par Unknown
 * The rationale for selecting this stream in individual callers is open.
 */
s32 gAdvanceSecondaryPrng(void)
{
    s32 i;
    s32 j;

    i = gFreeRangeCursor - 1;
    j = gFreeRangeCursor + 2;
    if (i < 0) {
        i = 0x13;
    } else if (j >= 0x14) {
        j = gFreeRangeCursor - 0x12;
    }
    gFreeRangeCursor = i;
    return gStateEntryTable[i] += gStateEntryTable[j];
}
