// Categories: Game Engine > State
#include "game/types.h"

extern s32 *gMovieSliceContext __asm__("D_8009F4BC");
void gSetMovieSlicePage(s32 value) __asm__("func_800858F4");


/**
 *  @brief Stores a value in a field of the globally referenced object.
 *
 *  The destination is the 32-bit field at byte offset 0x114 from the
 *  pointer held in gMovieSliceContext.
 *  @param value Value to store in the field.
 *  @par Globals
 *  gMovieSliceContext supplies the base pointer for the write.
 *  @par Control flow
 *  Performs one unconditional store.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the 32-bit field at the computed address.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetMovieSlicePage(s32 value)
{
    *(s32 *)((u8 *)gMovieSliceContext + 0x114) = value;
}
