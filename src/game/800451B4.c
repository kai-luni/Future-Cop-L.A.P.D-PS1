// Categories: Platform > Sound > Volume | Platform > Sound > Init
#include "game/types.h"

extern s32 D_8009F730;
extern s32 D_8009F334;

extern void func_80024730(s32 volume);

/**
 * @brief Sets the sound enable flag and re-applies the effect volume.
 *
 * D_8009F730 is the enable/mute word that func_800451E0 returns unchanged; func_80024730 is the mixer entry that applies a volume, and it is fed D_8009F334 - the effect volume - so that enabling or muting takes effect immediately.
 *
 * @param enable Non-zero enables sound, 0 mutes it; stored as a 0/1 flag.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F730 - enable flag, written as enable != 0. D_8009F334 - effect volume, read and handed to the mixer.
 *
 * @par Control flow
 * Normalise the argument to 0/1, one store, one call; no branches.
 *
 * @par Calls
 * func_80024730 - applies the effect volume to the mixer.
 *
 * @par Side effects
 * Mixer volumes change, so every playing effect is affected at once.
 *
 * @par Callers
 * Confirmed: func_80044F8C (0x80044F94) at sound init, func_800260B8 (0x80026594, 0x800265C4) and func_80080984 (0x80080998) from the options screen, func_8007CA34 (0x8007CA68) and the data block D_8007D27C (0x8007D2A4).
 */
void func_800451B4(s32 enable)
{
    D_8009F730 = enable != 0;
    func_80024730(D_8009F334);
}
