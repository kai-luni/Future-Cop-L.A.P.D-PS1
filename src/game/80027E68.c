// Categories: Game Engine > Interactive Loop | Graphics > Frame Timing
#include "game/types.h"

extern s32 D_8009F9FC;
extern s32 D_8009FA30;
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern s32 D_8009FAA4;

extern void func_80057864(void);

/**
 * @brief Waits until at least 5 ticks have elapsed since D_8009FA30 and updates frame delta.
 *
 * Polls func_80057864() (controller profile refresh) and D_8009FA60 until current - D_8009FA30
 * reaches 5 while updating D_8009FAA4, clamps the delta at 0x12C (300), and stores D_8009F9FC and D_8009FA30.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA60 - tick counter (read); D_8009FA30 - previous tick (updated); D_8009FAA4 - current tick; D_8009F9FC - clamped frame delta.
 *
 * @par Control flow
 * Do-while spin loop while diff < 5, followed by an upper clamp (diff >= 0x12D -> 0x12C).
 *
 * @par Calls
 * func_80057864 - refreshes the two controller/profile words in D_8009F7C0[0..1] on each poll pass.
 *
 * @par Side effects
 * Writes D_8009FAA4, D_8009F9FC, and D_8009FA30, plus D_8009F7C0 via func_80057864.
 *
 * @par Callers
 * Confirmed: func_80027EE4 (0x80027F34, called once per frame of the interactive UI loop) / Likely: none / Unknown: none.
 */
void func_80027E68(void)
{
    s32 diff;
    s32 current;

    do {
        func_80057864();
        current = gPaletteCycleCounter;
        diff = current - D_8009FA30;
        D_8009FAA4 = current;
    } while (diff < 5);

    if (diff >= 0x12D) {
        diff = 0x12C;
    }

    D_8009F9FC = diff;
    D_8009FA30 = current;
}
