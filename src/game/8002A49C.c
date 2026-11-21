// Categories: Game Engine > Queue
#include "game/types.h"

extern s32 gCallGateFlag __asm__("D_8009F6DC");
extern s32 gVagmReadIndex __asm__("D_8009F6CC");
extern u8 gVagmQueueDraining __asm__("D_8009F308");
extern s32 gVagmRing[] __asm__("D_800A0C50");
extern void gQueueStreamAssetRelease(s32 arg0, s32 arg1) __asm__("func_8002A31C");

/**
 *  @brief Processes consecutive nonzero entries in a four-slot queue.
 *
 *  When processing is enabled, marks the queue busy and consumes entries
 *  from the current cursor until it encounters an empty slot.
 *  @par Globals
 *  D_8009F6DC enables processing; D_800A0C50 holds the queue entries.
 *  D_8009F6CC is the wrapping cursor; D_8009F308 is the processing flag.
 *  @par Control flow
 *  If enabled, loops over nonzero entries and wraps the cursor at four.
 *  @par Calls
 *  func_8002A31C processes each entry with a second argument of zero.
 *  @par Side effects
 *  Clears processed slots, advances the cursor, and sets then clears the flag.
 *  @par Callers
 *  Confirmed: func_80029C34.
 */
void func_8002A49C(void)
{
    s32 temp;

    if (gCallGateFlag != 0) {
        temp = gVagmRing[gVagmReadIndex];
        gVagmQueueDraining = 1;
        while (temp != 0) {
            gQueueStreamAssetRelease(temp, 0);
            gVagmRing[gVagmReadIndex] = 0;
            gVagmReadIndex = gVagmReadIndex + 1;
            if (gVagmReadIndex >= 4) {
                gVagmReadIndex = 0;
            }
            temp = gVagmRing[gVagmReadIndex];
        }
        gVagmQueueDraining = 0;
    }
}
