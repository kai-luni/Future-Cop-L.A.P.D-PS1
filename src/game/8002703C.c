// Categories: Game Engine > Frame | Input > Controller | Graphics > Animation
#include "game/types.h"

struct Unk9F9A0 {
    u8 unk00[0x11C];
    u8 unk11C;
    u8 unk11D;
};

extern volatile s32 gGameClock __asm__("D_8009FA60");
extern volatile s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gDemoPlayback __asm__("D_8009FA84");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern u8 gFrameDeltaNewest __asm__("D_8009F690");
extern u8 gFrameDeltaPrev1 __asm__("D_8009F691");
extern u8 gFrameDeltaPrev2 __asm__("D_8009F692");
extern u8 gFrameDeltaRemainder __asm__("D_8009F693");
extern u8 *gFocusPositions[] __asm__("D_8009F9A0");

extern void func_80057864(void);
extern s32 gStepDemoInputPlayback(void) __asm__("func_80059DB8");
extern void gStepPendingMessages(s32 elapsed) __asm__("func_8004DB74");
extern void func_80063568(s32 arg);
extern void gStepAnimations(s32 arg) __asm__("func_800136C8");
extern void func_8004CC68(void);

void gStepGameplayFrameTime(void) __asm__("func_8002703C");

/**
 * @brief Waits for the next gameplay tick, smooths the frame step and advances timed
 * subsystems.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Waits for a tick difference of at least five, averages it with three signed history samples
 * and the saved remainder, then shifts the history. Applies a conditional object override, caps
 * the delta at 0x3C, samples controllers and replaces the delta and clocks during demo
 * playback. Passes the final step to entity and animation updates.
 *
 * @par Globals
 * Reads or writes gGameClock, gFrameTickBase, gFrameTime, gFrameDelta, gFrameDeltaNewest,
 * gFrameDeltaPrev1, gFrameDeltaPrev2, gFrameDeltaRemainder and gScreenState. Reads
 * gDemoPlayback and gFocusPositions.
 *
 * @par Callers
 * Direct calls from gRunGameFrameLoop (func_80028618).
 *
 * @par Role and path
 * The gameplay frame loop sets timing and samples input before running its render and
 * simulation passes.
 *
 * @par Unknown
 * The object bytes controlling the optional delta override are unresolved.
 */
void gStepGameplayFrameTime(void)
{
    s32 diff;
    register s32 sum __asm__("$2");
    s32 value;
    s32 nsum;
    struct Unk9F9A0 *obj;

    do {
        s32 cur = gGameClock;
        s32 prev = gFrameTickBase;
        diff = cur - prev;
        gFrameTime = cur;
    } while (diff < 5);

    __asm__ volatile("");
    sum = diff + (s8)gFrameDeltaNewest + (s8)gFrameDeltaPrev1;
    gFrameDelta = diff;
    sum = sum + (s8)gFrameDeltaPrev2 + (s8)gFrameDeltaRemainder;
    gFrameDelta = sum;
    gFrameDeltaRemainder = *(u8 *)&gFrameDelta & 3;
    if (sum < 0) {
        sum += 3;
    }
    __asm__ volatile("");
    obj = gFocusPositions[0];
    gFrameDelta = sum >> 2;
    gFrameDeltaPrev2 = gFrameDeltaPrev1;
    gFrameDeltaPrev1 = gFrameDeltaNewest;
    gFrameDeltaNewest = diff;
    if (obj != 0 && obj->unk11C != 0 && obj->unk11D != 0) {
        gFrameDelta = obj->unk11D * 5;
    }

    if (gFrameDelta >= 0x3C) {
        gFrameDelta = 0x3C;
    }

    func_80057864();
    if (gDemoPlayback > 0) {
        value = gStepDemoInputPlayback();
        gFrameDelta = value;
        if (value < 0) {
            gScreenState = 5;
            gFrameDelta = 5;
        }
        nsum = gFrameTickBase + gFrameDelta;
        gFrameTime = nsum;
        gGameClock = nsum;
    }

    gFrameTickBase = gFrameTime;
    gStepPendingMessages(gFrameDelta);
    func_80063568(gFrameDelta);
    gStepAnimations(gFrameDelta);
    func_8004CC68();
}
