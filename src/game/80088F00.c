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

extern Unk88F00 *gMovieStreamContext __asm__("D_8009F970");
extern Ent88F00 *gMovieStreamDescriptors __asm__("D_8009F974");
extern s32 gMovieRingStart __asm__("D_8009F978");
extern s32 gMovieRingEnd __asm__("D_8009F97C");

/**
 *  @brief Initializes the next table entry when the state is ready.
 *
 *  Selects an entry from the global array, initializes its fields, and advances
 *  the state index; exhaustion changes the state code to 2.
 *  @return The current state code, including any exhaustion update.
 *  @par Globals
 *  gMovieStreamContext holds the state and entry pointers; gMovieStreamDescriptors is the array base.
 *  gMovieRingStart and gMovieRingEnd supply two values copied into the entry.
 *  @par Control flow
 *  A nonzero state is returned unchanged; the first entry also resets totals.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the state, selected entry, and possibly the first-entry pointer.
 *  @par Callers
 *  Confirmed: gMovieStreamReaderInit.
 */
s32 gMovieStreamPrimeEntry(void) __asm__("func_80088F00");

s32 gMovieStreamPrimeEntry(void)
{
    if (gMovieStreamContext->unk43C == 0) {
        if (gMovieStreamContext->unk434 < gMovieStreamContext->unk438) {
            gMovieStreamContext->unk430 = gMovieStreamDescriptors + gMovieStreamContext->unk434;
            if (gMovieStreamContext->unk42C == 0) {
                gMovieStreamContext->unk42C = gMovieStreamContext->unk430;
            }
            gMovieStreamContext->unk430->w0 = (gMovieStreamContext->unk430->w10 + 0x7FF) >> 11;
            gMovieStreamContext->unk430->w4 = 0;
            gMovieStreamContext->unk430->w8 = 0;
            gMovieStreamContext->unk430->w28 = 0;
            if (gMovieStreamContext->unk434 == 0) {
                gMovieStreamContext->unk444 = 0;
                gMovieStreamContext->unk440 = 0;
                gMovieStreamContext->unk448 = 0;
                gMovieStreamContext->unk44C = 0;
            }
            gMovieStreamContext->unk434++;
            gMovieStreamContext->unk430->w34 = 0;
            gMovieStreamContext->unk430->w2C = gMovieRingStart;
            gMovieStreamContext->unk430->w30 = gMovieRingEnd;
            gMovieStreamContext->unk43C = 0;
        } else {
            gMovieStreamContext->unk43C = 2;
        }
        return gMovieStreamContext->unk43C;
    }
    return gMovieStreamContext->unk43C;
}
