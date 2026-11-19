// Categories: Platform > Sound | Platform > Streaming Audio
#include "game/types.h"

extern s32 gStreamCommandState __asm__("D_8009F648");

extern s32 func_800451E0(void);
extern void gApplyStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_8002283C");

void gSetVagmAudioStreamVolume(s32 volume) __asm__("func_80024730");

/**
 * @brief Applies a common playback level and stereo or mono panning to the two stream channels.
 *
 * @param volume Playback volume applied to both stream channels via gApplyStreamChannelMix.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Tests the stream command state, then calls the mode query. Passes the input level to channels
 * 0 and 1, with pans 0 and 0x7F in stereo or 0x40 for each in mono.
 *
 * @par Globals
 * Reads gStreamCommandState. Callees read the sound mode and apply mixer changes.
 *
 * @par Callers
 * Direct calls from func_800451B4, func_80045238.
 *
 * @par Role and path
 * The sound settings path updates the playing VAGM stream here.
 *
 * @par Unknown
 * The channel setter handles any clamping.
 */
void gSetVagmAudioStreamVolume(s32 volume)
{
    if (gStreamCommandState != 0) {
        if (func_800451E0() != 0) {
            gApplyStreamChannelMix(0, volume, 0);
            gApplyStreamChannelMix(1, volume, 0x7F);
        } else {
            gApplyStreamChannelMix(0, volume, 0x40);
            gApplyStreamChannelMix(1, volume, 0x40);
        }
    }
}
