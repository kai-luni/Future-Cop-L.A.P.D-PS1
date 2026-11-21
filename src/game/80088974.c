// Categories: Game Engine > State
#include "game/types.h"

extern u8 *gMovieStreamContext __asm__("D_8009F970");
extern void gQueueMovieCdControl(s32, s32, s32) __asm__("func_80088770");

/**
 *  @brief Stores two state values and invokes a helper with a third.
 *
 *  Writes two signed words into the structure addressed by gMovieStreamContext,
 *  then passes a fixed selector and the remaining argument to a helper.
 *  @param arg0 Value stored at byte offset 0x450.
 *  @param arg1 Value stored at byte offset 0x454.
 *  @param arg2 Value forwarded to the helper.
 *  @par Globals
 *  gMovieStreamContext supplies the base address for both stores.
 *  @par Control flow
 *  Performs both stores before the unconditional helper call.
 *  @par Calls
 *  func_80088770 is called with arguments 6, arg2, and 0.
 *  @par Side effects
 *  Updates two words in the pointed-to state; the helper may have other effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80088974(s32 arg0, s32 arg1, s32 arg2)
{
    s32 *base = (s32 *)gMovieStreamContext;

    base[0x114] = arg0;
    base[0x115] = arg1;
    gQueueMovieCdControl(6, arg2, 0);
}
