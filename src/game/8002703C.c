// Categories: Game Engine > Frame | Input > Controller | Graphics > Animation
#include "game/types.h"

struct Unk9F9A0 {
    u8 unk00[0x11C];
    u8 unk11C;
    u8 unk11D;
};

extern volatile s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern volatile s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gFrameId __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gModuleLoadedFlag __asm__("D_8009FA84");
extern s32 gDiscShadeByte __asm__("D_8009FAA4");
extern u8 gFrameDeltaNewest __asm__("D_8009F690");
extern u8 gFrameDeltaPrev1 __asm__("D_8009F691");
extern u8 gFrameDeltaPrev2 __asm__("D_8009F692");
extern u8 gFrameDeltaRemainder __asm__("D_8009F693");
extern u8 *gMissionWorkingSet[] __asm__("D_8009F9A0");

extern void func_80057864(void);
extern s32 func_80059DB8(void);
extern void func_8004DB74(s32 elapsed);
extern void func_80063568(s32 arg);
extern void func_800136C8(s32 arg);
extern void func_8004CC68(void);

/**
 * @brief Waits for minimum tick advance, smooths frame delta, samples input/demo state, and steps frame subsystems.
 *
 * Spins until D_8009FA60 - D_8009FA30 >= 5, computes a 4-sample smoothed delta in D_8009F9FC (using D_8009F690..D_8009F693, optional D_8009F9A0[0]->unk11D * 5 override, and cap 0x3C), samples pads via func_80057864, applies demo ticks via func_80059DB8 when D_8009FA84 > 0, and advances messages, animations, and lighting. Category Game Engine > Frame | Input > Controller | Graphics > Animation follows tick smoothing, pad sampling, and animation calls.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA60/D_8009FA30/D_8009FAA4/D_8009F9FC - tick counters and smoothed dt; D_8009F690..D_8009F693/D_8009F9A0/D_8009FA84/D_8009FA00 - history, override, demo, state.
 *
 * @par Control flow
 * Busy-wait do-while loop (diff < 5), straight-line averaging/clamping, conditional demo override, then sequential subsystem calls.
 *
 * @par Calls
 * func_80057864 - sample pads; func_80059DB8 - demo dt; func_8004DB74 - entity messages; func_80063568/func_800136C8 - animations; func_8004CC68 - pulse.
 *
 * @par Side effects
 * Updates frame-timing globals, D_8009F690..D_8009F693 history, and demo/animation/message/palette state via callees.
 *
 * @par Callers
 * Confirmed: func_80028618 (0x80028694 in asm/us/main.s; omitted from work list due to prologue offset) / Likely: none / Unknown: none.
 */
void func_8002703C(void)
{
    s32 diff;
    register s32 sum __asm__("$2");
    s32 value;
    s32 nsum;
    struct Unk9F9A0 *obj;

    do {
        s32 cur = gPaletteCycleCounter;
        s32 prev = gFrameTickBase;
        diff = cur - prev;
        gDiscShadeByte = cur;
    } while (diff < 5);

    __asm__ volatile("");
    sum = diff + (s8)gFrameDeltaNewest + (s8)gFrameDeltaPrev1;
    gFrameId = diff;
    sum = sum + (s8)gFrameDeltaPrev2 + (s8)gFrameDeltaRemainder;
    gFrameId = sum;
    gFrameDeltaRemainder = *(u8 *)&gFrameId & 3;
    if (sum < 0) {
        sum += 3;
    }
    __asm__ volatile("");
    obj = gMissionWorkingSet[0];
    gFrameId = sum >> 2;
    gFrameDeltaPrev2 = gFrameDeltaPrev1;
    gFrameDeltaPrev1 = gFrameDeltaNewest;
    gFrameDeltaNewest = diff;
    if (obj != 0 && obj->unk11C != 0 && obj->unk11D != 0) {
        gFrameId = obj->unk11D * 5;
    }

    if (gFrameId >= 0x3C) {
        gFrameId = 0x3C;
    }

    func_80057864();
    if (gModuleLoadedFlag > 0) {
        value = func_80059DB8();
        gFrameId = value;
        if (value < 0) {
            gScreenState = 5;
            gFrameId = 5;
        }
        nsum = gFrameTickBase + gFrameId;
        gDiscShadeByte = nsum;
        gPaletteCycleCounter = nsum;
    }

    gFrameTickBase = gDiscShadeByte;
    func_8004DB74(gFrameId);
    func_80063568(gFrameId);
    func_800136C8(gFrameId);
    func_8004CC68();
}
