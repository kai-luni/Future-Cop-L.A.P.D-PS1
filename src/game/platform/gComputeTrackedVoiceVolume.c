// Categories: Platform > Sound
#include "game/types.h"

extern s32 gGetVoiceVolume(void) __asm__("func_800452C8");
extern void gEnableHalfVolumeForSoundVoices(void) __asm__("func_800452D4");
s8 gComputeTrackedVoiceVolume(void) __asm__("func_80045050");


/**
 * @brief Computes the scaled voice volume from D_8009F338 and refreshes active voice parameters.
 *
 * Confirmed: queries volume via func_800452C8; when non-zero, scales by 1.5 (volume + (volume >> 1)), clamps to 0x7F, calls func_800452D4, and returns the s8 result. Likely: supplies boosted 7-bit volume to func_80023F08; Unknown: why 1.5x scaling is applied here.
 *
 * @param void No parameters.
 * @return Signed 8-bit volume in 0..0x7F.
 *
 * @par Globals
 * None directly (callees read D_8009F338, D_8009F74C, D_8009F754 and write D_8009F738).
 *
 * @par Control flow
 * Skips scaling, clamping at 0x7F, and voice refresh when func_800452C8 returns 0, then sign-extends the low byte.
 *
 * @par Calls
 * func_800452C8 - reads D_8009F338 volume word; func_800452D4 - sets D_8009F738 = 1 and updates D_8009F74C/D_8009F754 via func_8004530C.
 *
 * @par Side effects
 * Mutates sound voice state indirectly via func_800452D4 when volume is non-zero.
 *
 * @par Callers
 * Confirmed: func_80023F08 (0x80023F80 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s8 gComputeTrackedVoiceVolume(void)
{
    s32 volume;

    volume = gGetVoiceVolume();
    if (volume != 0) {
        volume += volume >> 1;
        if (volume >= 0x80) {
            volume = 0x7F;
        }
        gEnableHalfVolumeForSoundVoices();
    }
    return volume;
}
