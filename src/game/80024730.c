// Categories: Platform > Sound | Platform > Streaming Audio
#include "game/types.h"

extern s32 D_8009F648;

extern s32 func_800451E0(void);
extern void func_8002283C(s32 channel, s32 left, s32 right);

/**
 * @brief Applies volume and stereo/mono channel panning across stream channels 0 and 1.
 *
 * Confirmed: returns immediately when stream-active flag D_8009F648 == 0; otherwise queries stereo mode via func_800451E0 and calls func_8002283C on channels 0 and 1 with pan (0, 0x7F) when non-zero or centered pan (0x40, 0x40) when zero.
 *
 * @param volume Playback volume applied to both stream channels via func_8002283C.
 *
 * @par Globals
 * D_8009F648 - audio stream initialized/active flag (read; non-zero required to apply volume).
 *
 * @par Control flow
 * Outer guard on D_8009F648 != 0 enclosing an if/else branch on func_800451E0() != 0.
 *
 * @par Calls
 * func_800451E0 - reads D_8009F730 stereo/enable flag; func_8002283C - updates volume and pan on stream channels 0 and 1 in D_800A0968.
 *
 * @par Side effects
 * Updates stream channel volumes/pans in D_800A0968 and active voices via func_8002283C.
 *
 * @par Callers
 * Confirmed: func_800451B4 (0x800451C8 in asm/us/main.s), func_80045238 (0x80045270 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80024730(s32 volume)
{
    if (D_8009F648 != 0) {
        if (func_800451E0() != 0) {
            func_8002283C(0, volume, 0);
            func_8002283C(1, volume, 0x7F);
        } else {
            func_8002283C(0, volume, 0x40);
            func_8002283C(1, volume, 0x40);
        }
    }
}
