// Categories: Platform > Audio
#include "game/types.h"

extern u32 gSpuAccessVoiceMaskRegs(s32 on_off, u32 voice_bits, s32 low_register,
                         s32 high_register) __asm__("func_800987B8");

/**
 *  @brief Forwards a voice control request with a fixed register pair.
 *
 *  Passes the requested on/off setting and voice bit mask to the shared helper.
 *  The helper receives 0xCC and 0xCD as its low and high register arguments.
 *  @param on_off Requested on/off setting.
 *  @param voice_bits Bit mask identifying the voices.
 *  @return Result returned by the shared helper.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Makes one unconditional helper call and returns its result.
 *  @par Calls
 *  func_800987B8 handles the request using the supplied register pair.
 *  @par Side effects
 *  Any effects are those of the helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u32 func_80098788(s32 on_off, u32 voice_bits)
{
    return gSpuAccessVoiceMaskRegs(on_off, voice_bits, 0xCC, 0xCD);
}
