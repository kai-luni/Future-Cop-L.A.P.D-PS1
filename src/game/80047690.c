// Categories: Game Engine > Frame | Graphics > Display
#include "game/types.h"

extern u8 gStreamAudioFlag __asm__("D_8009F739");

extern void func_8002A16C(s32 value);
extern void gSoundHaltAllVoices(s32 value) __asm__("func_80047554");
void gSetAudioPlaybackEnabled(u8 enable) __asm__("func_80047690");


/**
 * @brief Selects between starting the stream state and halting sound voices.
 *
 * @param enable Nonzero selects the stream-state call; zero halts voices.
 * @return Nothing.
 *
 * @par Confirmed
 * Enable sets D_8009F739 = 1 and calls func_8002A16C(1); disable calls
 * gSoundHaltAllVoices(0). The callees may modify other state.
 *
 * @par Likely role
 * Frame and debug callers use both branches; the flag and callees belong to
 * the audio path, not a confirmed text-layer toggle.
 *
 * @par Unknown
 * What func_8002A16C/gSoundHaltAllVoices toggle beyond the layer flag.
 *
 * @par Callers
 * func_800259D8, func_80025B7C, func_80027510, func_80082DDC.
 */
void gSetAudioPlaybackEnabled(u8 enable)
{
    if (enable) {
        gStreamAudioFlag = 1;
        func_8002A16C(1);
    } else {
        gSoundHaltAllVoices(0);
    }
}
