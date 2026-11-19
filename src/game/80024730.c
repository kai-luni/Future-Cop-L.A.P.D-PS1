// Categories: Platform > Sound | Platform > Streaming Audio
#include "game/types.h"

extern s32 gStreamCommandState __asm__("D_8009F648");

extern s32 func_800451E0(void);
extern void gApplyStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_8002283C");

/**
 * @brief Applies volume and stereo/mono channel panning across stream channels 0 and 1.
 *
 * Confirmed: returns immediately when stream-active flag D_8009F648 == 0; otherwise queries stereo mode via func_800451E0 and calls gApplyStreamChannelMix on channels 0 and 1 with pan (0, 0x7F) when non-zero or centered pan (0x40, 0x40) when zero.
 *
 * @param volume Playback volume applied to both stream channels via gApplyStreamChannelMix.
 *
 * @par Globals
 * D_8009F648 - audio stream initialized/active flag (read; non-zero required to apply volume).
 *
 * @par Control flow
 * Outer guard on D_8009F648 != 0 enclosing an if/else branch on func_800451E0() != 0.
 *
 * @par Calls
 * func_800451E0 - reads D_8009F730 stereo/enable flag; gApplyStreamChannelMix - updates volume and pan on stream channels 0 and 1 in D_800A0968.
 *
 * @par Side effects
 * Updates stream channel volumes/pans in D_800A0968 and active voices via gApplyStreamChannelMix.
 *
 * @par Callers
 * Confirmed: func_800451B4 (0x800451C8 in asm/us/main.s), func_80045238 (0x80045270 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80024730(s32 volume)
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
