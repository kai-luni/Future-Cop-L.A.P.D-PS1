// Categories: Resources > Save Data | Platform > Sound > Volume | Game Engine > Front End
#include "game/types.h"

extern s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
extern s32 gGetMusicVolume(void) __asm__("func_8004522C");
extern s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
extern s32 gGetVoiceVolume(void) __asm__("func_800452C8");
extern s32 gIsPadRumbleEnabled(s32 arg0) __asm__("func_8001EB2C");

extern u8 gPresetIndexPerPort __asm__("D_8009FA98");
extern u8 gPort2ControllerPresetIndex[] __asm__("D_8009FA99");
void gPackAudioAndPadSettings(u8 *arg0) __asm__("func_80080A5C");


/**
 * @brief Snapshots current audio volumes, option bytes, and device settings into a save/option sub-block.
 *
 * Reads four sound volume/mixer getters, two global option bytes, and two entries from the controller/device table via func_8001EB2C, packing the 8 resulting bytes into arg0 + 0x04..0x0B.
 *
 * @param arg0 Destination block; bytes +0x04..+0x0B are overwritten.
 *
 * @par Globals
 * D_8009FA98 - option byte written to arg0[8]. D_8009FA99 - byte array whose index 0 is written to arg0[9].
 *
 * @par Control flow
 * Straight-line sequence of six getter calls and two global byte loads; no branches or loops.
 *
 * @par Calls
 * func_8004522C, func_80045288, func_800452C8, func_800451E0 - supply music, SFX, stream, and fourth audio bytes for +0x04..+0x07.
 * func_8001EB2C - reads byte +0x17 from device/controller table slots 0 and 1 into +0x0A and +0x0B.
 *
 * @par Side effects
 * Overwrites arg0[4] through arg0[0xB].
 *
 * @par Callers
 * Confirmed: func_80080B70 (0x80080BB4 for header options at +0x40C and 0x80080BDC for profile record audio block at +0x474).
 */
void gPackAudioAndPadSettings(u8 *arg0)
{
    arg0[4] = gGetMusicVolume();
    arg0[5] = gGetVagmVolumeSetting();
    arg0[6] = gGetVoiceVolume();
    arg0[7] = gGetStereoSoundMode();
    arg0[0xA] = gIsPadRumbleEnabled(0);
    arg0[0xB] = gIsPadRumbleEnabled(1);
    arg0[8] = gPresetIndexPerPort;
    arg0[9] = gPort2ControllerPresetIndex[0];
}
