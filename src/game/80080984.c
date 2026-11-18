// Categories: Resources > Save Data | Platform > Sound > Volume | Input > Port Presets
#include "game/types.h"

extern void func_800451B4(s32 enable);
extern void func_800451F8(s32 volume);
extern void func_80045238(s32 volume);
extern void func_80045294(s32 volume);
extern void func_8001EC04(s32 port, s32 enable);
extern s32 func_80045288(void);
extern s32 func_8002A58C(void);
extern void func_80024630(s32 arg0);
extern void func_8002A16C(s32 arg0);
extern void gApplyControllerPreset(s32 port, s32 value) __asm__("func_8007BFCC");

/**
 * @brief Restores audio volumes, device enables, and controller presets from a saved option sub-block.
 *
 * Applies bytes arg0 + 0x04..0x0B (mirror of func_80080A5C) to the mixer/volume setters, controller/device toggles, and both input port presets, then starts or stops stream playback to match the resulting SFX volume.
 *
 * @param arg0 Source option block (+0x04..+0x07 audio bytes, +0x08/+0x09 signed port presets, +0x0A/+0x0B device enables).
 * @param arg1 Unused parameter.
 *
 * @par Globals
 * None accessed directly.
 *
 * @par Control flow
 * Applies all eight configuration setters sequentially, then checks func_80045288() == 0 vs (func_8002A58C() & 0xFF): stops playback via func_80024630(0) if volume is 0 while state == 1, or starts it via func_8002A16C(1) if volume != 0 while state == 0.
 *
 * @par Calls
 * func_800451B4/func_800451F8/func_80045238/func_80045294 (apply audio settings +0x07/+0x04..+0x06), func_8001EC04 (device slots 0/1 from +0x0A/+0x0B), func_8007BFCC (port presets 0/1 from +0x08/+0x09), func_80045288 & func_8002A58C (query volume and stream state), func_80024630 & func_8002A16C (stop/start stream playback).
 *
 * @par Side effects
 * Updates global audio volumes, device/controller port configs, and stream playback state.
 *
 * @par Callers
 * Confirmed: func_80080AD4 (0x80080AF0, applies D_8009F92C + 0x40C when byte +0x923 is set) and func_8008125C (0x80081694, applies selected profile record +0x474).
 */
void gApplySavedAudioSettings(u8 *arg0, s32 arg1) __asm__("func_80080984");

void gApplySavedAudioSettings(u8 *arg0, s32 arg1)
{
    func_800451B4(arg0[7]);
    func_800451F8(arg0[4]);
    func_80045238(arg0[5]);
    func_80045294(arg0[6]);
    func_8001EC04(0, arg0[0xA]);
    func_8001EC04(1, arg0[0xB]);
    gApplyControllerPreset(0, *(s8 *)(arg0 + 8));
    gApplyControllerPreset(1, *(s8 *)(arg0 + 9));

    if (func_80045288() == 0) {
        if ((func_8002A58C() & 0xFF) == 1) {
            func_80024630(0);
        }
    } else {
        if ((func_8002A58C() & 0xFF) == 0) {
            func_8002A16C(1);
        }
    }
}
