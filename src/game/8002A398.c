// Categories: Game Engine > Queue
#include "game/types.h"

typedef struct Unk8002A398 {
    u8 unk00[0x14];
    u16 unk14;
    u16 unk16;
} Unk8002A398;

extern u8 gVagmQueueDraining __asm__("D_8009F308");
extern s32 gVagmReadIndex __asm__("D_8009F6CC");
extern u32 gVagmNextChunkIndex __asm__("D_8009F6E4");
extern Unk8002A398 *gVagmRing[4] __asm__("D_800A0C50");
s32 gDequeueVagmChunk(void) __asm__("func_8002A398");


/**
 *  @brief Removes the current entry from a four-slot pointer queue.
 *
 *  When enabled and the current slot is occupied, updates a derived index
 *  from the entry, clears the slot, and advances the queue cursor.
 *  @return The removed pointer cast to s32, or zero if none was removed.
 *  @par Globals
 *  D_8009F308 gates removal; D_800A0C50 holds the four pointers.
 *  D_8009F6CC is the cursor; D_8009F6E4 receives the derived index.
 *  @par Control flow
 *  Checks the gate and slot, then wraps the derived index and cursor as needed.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May clear a queue slot and update both global indices.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
s32 gDequeueVagmChunk(void)
{
    Unk8002A398 *ptr;

    ptr = 0;
    if (gVagmQueueDraining != 1) {
        ptr = gVagmRing[gVagmReadIndex];
        if (ptr != 0) {
            gVagmNextChunkIndex = ptr->unk16 + 1;
            if (gVagmNextChunkIndex >= ptr->unk14) {
                gVagmNextChunkIndex = 0;
            }
            gVagmRing[gVagmReadIndex] = 0;
            gVagmReadIndex++;
            if (gVagmReadIndex >= 4) {
                gVagmReadIndex = 0;
            }
        }
    }
    return (s32)ptr;
}
