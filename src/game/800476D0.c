// Categories: Platform > Sound | Game Engine > Frame
#include "game/types.h"

extern u8 func_80024170(void);
extern u8 func_80029BC0(void);
extern s32 func_80024188(void);
extern void gSoundHaltAllVoices(s32 value) __asm__("func_80047554");

/**
 * @brief Silences active sound voices on pause, optionally preserving one background stream voice.
 *
 * Checks whether the tracked audio flag D_800A0A88 is active and the current
 * stream track D_8009F6BC differs from D_800A1800; if so, passes the valid
 * D_800A0A80 resource node to gSoundHaltAllVoices so its SPU voice stays unmuted, else passes 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0A88, D_800A0A80, D_8009F6BC - read indirectly via callees to resolve the preserved SoundVoice node.
 * @par Control flow
 * Defaults value to 0, evaluates short-circuit guard (func_80024170() && !func_80029BC0()), conditionally calls func_80024188(), then calls gSoundHaltAllVoices(value).
 * @par Calls
 * func_80024170 - tests D_800A0A88 == 1; func_80029BC0 - tests if D_8009F6BC matches D_800A1800;
 * func_80024188 - validates D_800A0A80 via func_800474C4 and returns its resource pointer;
 * gSoundHaltAllVoices - stops queued/active SoundVoice lists and mutes SPU voices (via func_80021BD0 or func_80021B90).
 * @par Side effects
 * Tears down active sound voices and updates SPU voice volumes via gSoundHaltAllVoices.
 * @par Callers
 * Confirmed: func_80027510 (two-player pause entry); also direct call at 0x800274D4 in asm/us/main.s (single-player pause entry).
 */
void gSoundPauseVoices(void) __asm__("func_800476D0");

void gSoundPauseVoices(void)
{
    s32 value;

    value = 0;
    if (func_80024170() != 0 && func_80029BC0() == 0) {
        value = func_80024188();
    }
    gSoundHaltAllVoices(value);
}
