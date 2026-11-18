// Categories: Game Engine > State Transitions
#include "game/types.h"

extern u8 gPauseFlag __asm__("D_8009F3E8");

extern void func_8007F5D0(void);

/**
 *  @brief Sets a flag and invokes a helper when the input equals 3.
 *
 *  Other input values leave the flag unchanged and skip the helper.
 *  The flag is set before the helper is called.
 *  @param a0 Value tested against 3.
 *  @par Globals
 *  D_8009F3E8 is set to 1 when a0 equals 3.
 *  @par Control flow
 *  A single conditional branch gates the store and call.
 *  @par Calls
 *  func_8007F5D0 runs after the flag is set.
 *  @par Side effects
 *  May update D_8009F3E8 and produce effects from the helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gRequestPauseMenu(s32 a0) __asm__("func_8007F648");

void gRequestPauseMenu(s32 a0)
{
    if (a0 == 3) {
        gPauseFlag = 1;
        func_8007F5D0();
    }
}
