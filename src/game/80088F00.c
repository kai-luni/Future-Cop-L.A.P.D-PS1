// Categories: Game Engine > State
#include "game/types.h"

typedef struct Ent88F00 {
    u32 w0;
    s32 w4;
    s32 w8;
    s32 padC;
    u32 w10;
    u8 pad14[0x14];
    s32 w28;
    s32 w2C;
    s32 w30;
    s32 w34;
} Ent88F00;

typedef struct {
    u8 pad0[0x42C];
    Ent88F00 *unk42C;
    Ent88F00 *unk430;
    u32 unk434;
    u32 unk438;
    s32 unk43C;
    s32 unk440;
    s32 unk444;
    s32 unk448;
    s32 unk44C;
} Unk88F00;

extern Unk88F00 *gQueueBlock __asm__("D_8009F970");
extern Ent88F00 *gStreamDescriptors __asm__("D_8009F974");
extern s32 D_8009F978;
extern s32 D_8009F97C;

/**
 *  @brief Initializes the next table entry when the state is ready.
 *
 *  Selects an entry from the global array, initializes its fields, and advances
 *  the state index; exhaustion changes the state code to 2.
 *  @return The current state code, including any exhaustion update.
 *  @par Globals
 *  D_8009F970 holds the state and entry pointers; D_8009F974 is the array base.
 *  D_8009F978 and D_8009F97C supply two values copied into the entry.
 *  @par Control flow
 *  A nonzero state is returned unchanged; the first entry also resets totals.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the state, selected entry, and possibly the first-entry pointer.
 *  @par Callers
 *  Confirmed: func_80089700.
 */
s32 func_80088F00(void)
{
    if (gQueueBlock->unk43C == 0) {
        if (gQueueBlock->unk434 < gQueueBlock->unk438) {
            gQueueBlock->unk430 = gStreamDescriptors + gQueueBlock->unk434;
            if (gQueueBlock->unk42C == 0) {
                gQueueBlock->unk42C = gQueueBlock->unk430;
            }
            gQueueBlock->unk430->w0 = (gQueueBlock->unk430->w10 + 0x7FF) >> 11;
            gQueueBlock->unk430->w4 = 0;
            gQueueBlock->unk430->w8 = 0;
            gQueueBlock->unk430->w28 = 0;
            if (gQueueBlock->unk434 == 0) {
                gQueueBlock->unk444 = 0;
                gQueueBlock->unk440 = 0;
                gQueueBlock->unk448 = 0;
                gQueueBlock->unk44C = 0;
            }
            gQueueBlock->unk434++;
            gQueueBlock->unk430->w34 = 0;
            gQueueBlock->unk430->w2C = D_8009F978;
            gQueueBlock->unk430->w30 = D_8009F97C;
            gQueueBlock->unk43C = 0;
        } else {
            gQueueBlock->unk43C = 2;
        }
        return gQueueBlock->unk43C;
    }
    return gQueueBlock->unk43C;
}
