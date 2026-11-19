// Categories: Platform > Sound > Voices
#include "game/types.h"

extern void func_80021A8C(u32 mask);

void gQueueSoundChannelRelease(s32 channel) __asm__("func_8002238C");

/**
 * @brief Queues a sound channel for release and clears its pending enable bit.
 *
 * @param channel Zero-based sound channel index (0..31).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Uses SLLV to form the channel bit and passes it in the call delay slot to func_80021A8C. That
 * helper clears the bit at gSoundSystem +0x14 and sets it at +0x18.
 *
 * @par Globals
 * None directly. The callee updates both masks in gSoundSystem.
 *
 * @par Callers
 * Direct calls from gReleaseSoundVoicesByOwner (func_80022240), func_800243F8.
 *
 * @par Role and path
 * Software voice release and descriptor-based stop both enqueue this update.
 *
 * @par Unknown
 * The helper does not apply the queued change to hardware itself.
 */
void gQueueSoundChannelRelease(s32 channel)
{
    func_80021A8C(1 << channel);
}
