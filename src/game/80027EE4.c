// Categories: Game Engine > Interactive Loop | Graphics > Frame Timing
#include "game/types.h"

extern s32 gGlobalTick __asm__("D_8009F284");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gSceneFrameCounter __asm__("D_8009FA50");
extern volatile s32 gGameClock __asm__("D_8009FA60");
extern s32 gFrameTime __asm__("D_8009FAA4");

extern void gWaitForInteractiveFrameTicks(void) __asm__("func_80027E68");
extern void gPresentInteractiveScreenFrame(void) __asm__("func_80027EBC");
extern void func_800451EC(s32 value);
extern void gUpdateSoundVoiceAssignments(void) __asm__("func_800462D0");
extern void gUpdateAndDrawFrontEndMenu(void) __asm__("func_80078DD0");

void gRunInteractiveScreenLoop(void) __asm__("func_80027EE4");

/**
 * @brief Runs the interactive screen frames while the screen state is 4.
 *
 * @param void No parameters.
 * @return Nothing; it returns when the state leaves 4, i.e. when the screen decides to end.
 *
 * @par Confirmed mechanics
 * Clears time and frame counters and sets the initial delta to 8. Each iteration increments the
 * two counters, waits for a frame tick, services audio and screen drawing, dispatches assets
 * and presents before rechecking the screen state.
 *
 * @par Globals
 * Reads gScreenState. Initializes gGameClock, gFrameTime, gFrameTickBase, gFrameDelta and
 * gSceneFrameCounter, and increments gSceneFrameCounter and gGlobalTick.
 *
 * @par Callers
 * Direct calls from gRunScreenModule (func_800287F4).
 *
 * @par Role and path
 * The screen module takes this loop for its interactive path.
 *
 * @par Unknown
 * The loop exits on any transition away from state 4.
 */
void gRunInteractiveScreenLoop(void)
{
    s32 t;
    gGameClock = 0;
    t = gGameClock;
    gFrameDelta = 8;
    gFrameTime = t;
    gFrameTickBase = t;
    gSceneFrameCounter = 0;

    while (gScreenState == 4) {
        gSceneFrameCounter++;
        gGlobalTick++;
        gWaitForInteractiveFrameTicks();
        func_800451EC(0);
        gUpdateSoundVoiceAssignments();
        gUpdateAndDrawFrontEndMenu();
        gPresentInteractiveScreenFrame();
    }
}
