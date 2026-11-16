// Categories: Platform > Sound > Volume
#include "game/types.h"

extern s32 D_8009F330;

/**
 * @brief Sets the music volume, clamped to the 0..0x7F mixer range.
 *
 * Negative input becomes 0 and 0x80 and above saturate at 0x7F, so the stored word always fits the 7-bit range the options screen works in. Unlike the effect-volume setter func_80045238 this one only records the value; the getter is func_8004522C.
 *
 * @param volume Requested music volume.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F330 - music volume word, written with the clamped value.
 *
 * @par Control flow
 * Two comparisons select among three stores (volume, 0x7F, 0); no loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Only D_8009F330 changes; no hardware register is touched, so playback picks the value up when the mixer next applies volumes.
 *
 * @par Callers
 * Confirmed: func_80044F8C (0x80044FA4) at sound init with 0x40, func_800260B8 (0x800264C8) and func_80080984 (0x800809A4) from the options screen, func_8007CA34 (0x8007CA44), func_8007CACC (0x8007CBA0, 0x8007CC30).
 */
void func_800451F8(s32 volume)
{
    if (volume >= 0) {
        if (volume < 0x80) {
            D_8009F330 = volume;
        } else {
            D_8009F330 = 0x7F;
        }
    } else {
        D_8009F330 = 0;
    }
}
