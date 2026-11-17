// Categories: Platform > Sound > Volume | Game Engine > Front-End Configuration
#include "game/types.h"

extern s32 gMusicVolumeSetting __asm__("D_8009F334");

extern void func_80024730(s32 volume);

/**
 * @brief Clamps a volume request to 0..0x7F, stores it and applies it to both
 *        output channels.
 *
 * D_8009F334 is the second of the three volume words of the options screen; the
 * getter is func_80045288 and the third word is read by func_800452C8. The clamp
 * is asymmetric because the value ends up in a signed mixer argument: anything
 * below zero becomes 0 and anything at or above 0x80 saturates at 0x7F.
 *
 * @param volume Requested volume, clamped to 0..0x7F before it is stored.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F334 - the stored volume word; it is read back and forwarded, so the
 * mixer is always fed the clamped value.
 *
 * @par Control flow
 * Two nested signed compares (negative, then >= 0x80) select one of three
 * stores, followed by a single call.
 *
 * @par Calls
 * func_80024730 - applies the value to channels 0 and 1 via func_8002283C, with
 * an asymmetric left/right pair when func_800451E0 reports headphones; it does
 * nothing at all when D_8009F648 is 0.
 *
 * @par Side effects
 * Writes D_8009F334 and, through the callee, the channel volumes.
 *
 * @par Callers
 * Confirmed: func_800260B8, func_80044F8C, func_8007CA34, func_8007CACC,
 * func_80080984.
 */
void func_80045238(s32 volume)
{
    if (volume >= 0) {
        if (volume < 0x80) {
            gMusicVolumeSetting = volume;
        } else {
            gMusicVolumeSetting = 0x7F;
        }
    } else {
        gMusicVolumeSetting = 0;
    }
    func_80024730(gMusicVolumeSetting);
}
