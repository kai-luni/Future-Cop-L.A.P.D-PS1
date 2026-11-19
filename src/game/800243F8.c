// Categories: Platform > Sound > Voices
#include "game/types.h"

extern void gQueueSoundChannelRelease(s32 arg0) __asm__("func_8002238C");

/**
 * @brief Releases the hardware sound channel bound to a voice descriptor.
 *
 * Reads the signed channel index at arg0 + 0x0D and passes it to gQueueSoundChannelRelease, which keys off/stops channel mask (1 << channel) via func_80021A8C.
 *
 * @param arg0 Voice descriptor pointer containing the signed channel byte at +0x0D.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line signed byte load from arg0 + 0x0D and tail call to gQueueSoundChannelRelease.
 *
 * @par Calls
 * gQueueSoundChannelRelease - stops SPU/driver channel (1 << channel) via func_80021A8C.
 *
 * @par Side effects
 * Stops the hardware sound channel associated with the voice record.
 *
 * @par Callers
 * Confirmed: func_80046698 (0x800466B8), func_80047088 (0x80047124), func_800471C4 (0x80047228).
 */
void func_800243F8(s8 *arg0) {
    gQueueSoundChannelRelease(*(arg0 + 0xD));
}
