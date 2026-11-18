// Categories: Game Engine > Buffer
#include "game/types.h"

typedef struct {
    volatile u32 start;
    u32 end;
    u8 unk08[0x7B70];
    u32 unk7B78;
} Unk800869D8;

extern Unk800869D8 *gMovieAudioState __asm__("D_8009F4D8");

/**
 *  @brief Returns a pointer into the first buffer region when available.
 *
 *  Checks the requested index against the unsigned end-minus-start range
 *  and requires the state flag to be zero; the index does not offset the pointer.
 *  @param index Index used for the availability check.
 *  @return Pointer at start modulo 15360 plus 0x10, or null if unavailable.
 *  @par Globals
 *  gMovieAudioState points to the buffer state and storage.
 *  @par Control flow
 *  Returns the computed pointer only when both checks pass.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the volatile start field is read.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void *func_800869D8(u32 index)
{
    Unk800869D8 *p = gMovieAudioState;
    return (index < p->end - p->start && p->unk7B78 == 0) ? (u8 *)p + ((p->start % 15360) + 0x10) : 0;
}
