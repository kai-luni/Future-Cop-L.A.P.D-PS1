// Categories: Platform > Sound > Voices
#include "game/types.h"

extern void gQueueSoundChannelRelease(s32 arg0) __asm__("func_8002238C");

void gQueueVoiceChannelRelease(s8 *arg0) __asm__("func_800243F8");

/**
 * @brief Queues release of the channel selected by a voice descriptor.
 *
 * @param arg0 Voice descriptor pointer containing the signed channel byte at +0x0D.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the signed channel byte at descriptor +0x0D and calls gQueueSoundChannelRelease.
 *
 * @par Globals
 * None directly. The callee changes gSoundSystem channel masks.
 *
 * @par Callers
 * Direct calls from func_80046698, func_80047088, func_800471C4.
 *
 * @par Role and path
 * Used by voice cleanup and removal paths before the descriptor is retired.
 *
 * @par Unknown
 * The wrapper does not validate the descriptor.
 */
void gQueueVoiceChannelRelease(s8 *arg0) {
    gQueueSoundChannelRelease(*(arg0 + 0xD));
}
