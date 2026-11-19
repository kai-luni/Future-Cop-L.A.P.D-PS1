// Categories: Game Engine > State
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");

extern void gFlushSoundChannelUpdates(void) __asm__("func_80021E1C");

/**
 *  @brief Calls a helper only when a global flag is nonzero.
 *
 *  Tests the flag before dispatching to the helper.
 *  A zero flag suppresses the call.
 *  @par Globals
 *  D_8009F26C is read as the enable flag.
 *  @par Control flow
 *  One conditional determines whether the helper is called.
 *  @par Calls
 *  gFlushSoundChannelUpdates is invoked when the flag is nonzero.
 *  @par Side effects
 *  Any effects are those of the conditional helper call.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80023314(void)
{
    if (gCvkSystemActive != 0) {
        gFlushSoundChannelUpdates();
    }
}
