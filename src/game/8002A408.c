// Categories: Game Engine > Queue
#include "game/types.h"

extern s32 D_8009F6D0;
extern s32 gStreamAssetReleaseRing[] __asm__("D_800A0C60");

extern void func_80029650(s32 arg0);
void gDrainCallbackSlotRing(void) __asm__("func_8002A408");


/**
 *  @brief Processes consecutive nonzero entries in a four-slot circular table.
 *
 *  Starting at the shared index, each entry is passed to a handler and cleared.
 *  Processing stops at the first zero entry, leaving the index at that slot.
 *  @par Globals
 *  D_800A0C60 holds the entries; D_8009F6D0 is the current index.
 *  @par Control flow
 *  Loops over nonzero entries and wraps the index to zero at four.
 *  @par Calls
 *  func_80029650 handles each nonzero entry.
 *  @par Side effects
 *  Clears processed entries and advances the shared index.
 *  @par Callers
 *  Confirmed: func_80029C34, func_8002A26C.
 */
void gDrainCallbackSlotRing(void)
{
    s32 temp = gStreamAssetReleaseRing[D_8009F6D0];

    while (temp != 0) {
        func_80029650(temp);
        gStreamAssetReleaseRing[D_8009F6D0] = 0;
        D_8009F6D0 = D_8009F6D0 + 1;
        if (D_8009F6D0 >= 4) {
            D_8009F6D0 = 0;
        }
        temp = gStreamAssetReleaseRing[D_8009F6D0];
    }
}
