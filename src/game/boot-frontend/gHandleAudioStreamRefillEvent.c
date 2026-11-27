// Categories: Game Engine > Dispatch
#include "game/types.h"

extern int gReplaceVagmAudioStreamBuffer() __asm__("func_800246A8");

extern s32 gStreamBlockDeficitScale __asm__("D_8009F64C");

void gHandleAudioStreamRefillEvent(s32 arg0) __asm__("func_80024470");

/**
 * @brief Requests a replacement stream buffer for event codes 3 and 4.
 *
 * @param arg0 Signed value to test and potentially forward.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Uses two signed SLTI checks to admit 3 and 4, then calls gReplaceVagmAudioStreamBuffer with
 * the event argument.
 *
 * @par Globals
 * None directly. The replacement helper accesses gActiveStreamId.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * The configurator registers this callback through the address gScaleAudioStreamBlockDeficit
 * +0x34.
 *
 * @par Unknown
 * The distinction between event 3 and event 4 is unused by this handler.
 */
void gHandleAudioStreamRefillEvent(s32 arg0)
{
    if (arg0 < 5) {
        if (arg0 >= 3) {
            gReplaceVagmAudioStreamBuffer(arg0);
        }
    }
}
