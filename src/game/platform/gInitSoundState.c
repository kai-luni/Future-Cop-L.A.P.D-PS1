// Categories: Platform > Sound | Graphics > Sprite/Asset Subsystem
#include "game/types.h"

extern s32 gVolumeLevels[] __asm__("D_800A1800");
extern u8 gStreamAudioFlag __asm__("D_8009F739");
extern u8 gSoundProcessingEnabled __asm__("D_8009F340");

extern void gSetStereoSoundMode(s32 value) __asm__("func_800451B4");
extern void gSetSoundSpatialMode(s32 value) __asm__("func_800451EC");
extern void gSetSfxVolume(s32 value) __asm__("func_800451F8");
extern void gSetVagmVolumeSetting(s32 value) __asm__("func_80045238");
extern void gSetVoiceVolume(s32 value) __asm__("func_80045294");
extern void gInitSoundPools(void) __asm__("func_800453B8");
void gInitSoundState(void) __asm__("func_80044F8C");


/**
 * @brief Initializes the sound state: enable, three level parameters at
 *        0x40 and an empty slot list.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_800451B4(1); func_800451EC(0); func_800451F8(0x40);
 * func_80045238(0x40); func_80045294(0x40); func_800453B8();
 * D_800A1800[0] = -1; D_8009F739 = 0; D_8009F340 = 1.
 * func_800451F8 is a proven volume setter: it clamps its argument to
 * 0..0x7F and stores it in D_8009F330.
 *
 * @par Likely role
 * The two sibling calls with the same 0x40 argument set further sound
 * levels (the 0x7F clamp is the library's volume range); D_800A1800[0] = -1
 * marks the slot list empty.
 *
 * @par Callers
 * func_8002333C.
 *
 * @par Environment path
 * Not on the draw path: initializes the sound/audio subsystem state by
 * enabling audio and setting volume/stream parameters via func_800451B4,
 * func_800451EC, func_800451F8, func_80045238, func_80045294, and
 * func_800453B8 (called by func_8002333C).
 */
void gInitSoundState(void)
{
    gSetStereoSoundMode(1);
    gSetSoundSpatialMode(0);
    gSetSfxVolume(0x40);
    gSetVagmVolumeSetting(0x40);
    gSetVoiceVolume(0x40);
    gInitSoundPools();
    gVolumeLevels[0] = -1;
    gStreamAudioFlag = 0;
    gSoundProcessingEnabled = 1;
}
