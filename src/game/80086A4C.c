// Categories: Game Engine > Buffer
#include "game/types.h"

typedef struct {
    volatile u32 unk00;
    u32 unk04;
    u8 unk08[0x7B70];
    s32 unk7B78;
} Unk80086A4C;

extern Unk80086A4C *gMovieAudioState __asm__("D_8009F4D8");
void *gPeekMovieAudioSecondBuffer(u32 arg) __asm__("func_80086A4C");


/**
 *  @brief Returns a pointer into the second buffer region when available.
 *
 *  Checks the argument against the unsigned difference of the state bounds
 *  and requires the state flag to be zero; the argument does not offset the pointer.
 *  @param arg Index used for the availability check.
 *  @return Pointer at unk00 modulo 0x3c00 plus 0x3c10, or null if unavailable.
 *  @par Globals
 *  gMovieAudioState points to the buffer state and storage.
 *  @par Control flow
 *  Returns the computed pointer only when both checks pass.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the volatile unk00 field is read.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void *gPeekMovieAudioSecondBuffer(u32 arg)
{
    Unk80086A4C *p;

    p = gMovieAudioState;
    return (arg < p->unk04 - p->unk00 && p->unk7B78 == 0)
        ? (u8 *)p + (p->unk00 % 0x3C00 + 0x3C10)
        : 0;
}
