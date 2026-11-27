// Categories: Game Engine > State Access
#include "game/types.h"

extern u8 gCompletionFlag __asm__("D_8009FA68");
extern s32 gFileSlotState __asm__("D_8009F620");
void gFileReadDoneCallback(s32 arg0) __asm__("func_8001DFF0");


/**
 *  @brief Sets a global flag and stores a signed value.
 *
 *  Marks the associated state with a byte value of one and records
 *  the supplied value in a separate global.
 *  @param arg0 Signed value to store.
 *  @par Globals
 *  D_8009FA68 is set to one; D_8009F620 receives arg0.
 *  @par Control flow
 *  Performs both assignments unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the two globals.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Sets gCompletionFlag (D_8009FA68) to 1 and stores arg0 into gFileSlotState
 * (D_8009F620) with no outgoing calls. Passed as the completion callback to
 * func_8001D90C by func_8001E004 to unblock synchronous file reads.
 */
void gFileReadDoneCallback(s32 arg0)
{
    gCompletionFlag = 1;
    gFileSlotState = arg0;
}
