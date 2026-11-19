// Categories: Game Engine > Dispatch
#include "game/types.h"

extern s32 gWriteCdAudioMixRegs(u8 *arg0) __asm__("func_8008F650");
s32 gSetCdVolume(u8 *arg0) __asm__("func_8008E578");


/**
 *  @brief Passes a byte pointer to func_8008F650 and returns 1.
 *
 *  The callee's return value is discarded; this wrapper reports the
 *  same result regardless of that value.
 *  @param arg0 Byte pointer passed to the callee.
 *  @return Always 1.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Makes one unconditional call, then returns a constant.
 *  @par Calls
 *  func_8008F650 receives arg0 and performs the delegated operation.
 *  @par Side effects
 *  Any effects of func_8008F650 are not specified by this source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Passes arg0 to func_8008F650 to program the four CD-ROM audio attenuation
 * registers in banks 2 and 3 and always returns 1. Accesses no globals
 * directly and has no direct callers in src/.
 */
s32 gSetCdVolume(u8 *arg0)
{
    gWriteCdAudioMixRegs(arg0);
    return 1;
}
