// Categories: Game Engine > State Access
#include "game/types.h"

extern u8 gCompletionFlag __asm__("D_8009FA68");
extern s32 gFileSlotState __asm__("D_8009F620");

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
 */
void func_8001DFF0(s32 arg0)
{
    gCompletionFlag = 1;
    gFileSlotState = arg0;
}
