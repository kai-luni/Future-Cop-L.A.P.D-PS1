// Categories: Game Engine > Interactive Loop | Graphics > Frame Timing
#include "game/types.h"

extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gGameClock __asm__("D_8009FA60");
extern s32 gFrameTime __asm__("D_8009FAA4");

extern void func_80057864(void);

void gWaitForInteractiveFrameTicks(void) __asm__("func_80027E68");

/**
 * @brief Polls input while waiting for five ticks and then records a capped frame delta.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls func_80057864 on each poll, copies the current tick and subtracts the previous tick
 * until the difference reaches five. Caps the stored delta at 0x12C and advances the base tick.
 *
 * @par Globals
 * Reads gGameClock and reads and updates gFrameTickBase. Writes gFrameTime and gFrameDelta. The
 * input callee updates gPadWords.
 *
 * @par Callers
 * Direct calls from gRunInteractiveScreenLoop (func_80027EE4).
 *
 * @par Role and path
 * The interactive screen loop uses this pacing routine before each frame's audio and display
 * passes.
 *
 * @par Unknown
 * The cap applies to the stored frame delta, while the base stores the full current tick.
 */
void gWaitForInteractiveFrameTicks(void)
{
    s32 diff;
    s32 current;

    do {
        func_80057864();
        current = gGameClock;
        diff = current - gFrameTickBase;
        gFrameTime = current;
    } while (diff < 5);

    if (diff >= 0x12D) {
        diff = 0x12C;
    }

    gFrameDelta = diff;
    gFrameTickBase = current;
}
