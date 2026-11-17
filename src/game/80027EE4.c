// Categories: Game Engine > Interactive Loop | Graphics > Frame Timing
#include "game/types.h"

extern s32 D_8009F284;
extern s32 gFrameId __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 D_8009FA30;
extern s32 gSceneFrameCounter __asm__("D_8009FA50");
extern volatile s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern s32 D_8009FAA4;

extern void func_80027E68(void);
extern void func_80027EBC(void);
extern void func_800451EC(s32 value);
extern void func_800462D0(void);
extern void func_80078DD0(void);

/**
 * @brief Runs the interactive frame loop of a screen while the state stays 4.
 *
 * Sets up the frame bookkeeping (D_8009FA60 = 0, frame id D_8009F9FC = 8,
 * D_8009FAA4/D_8009FA30 = 0, D_8009FA50 = 0) and then loops as long as
 * D_8009FA00 == 4, each round incrementing D_8009FA50 (frame counter) and
 * D_8009F284 (global tick), and calling, in this order: func_80027E68(),
 * func_800451EC(0), func_800462D0(), func_80078DD0(), func_80027EBC().
 *
 * @return Nothing; it returns when the state leaves 4, i.e. when the screen
 *         decides to end.
 *
 * @par globals
 * D_8009FA00 (state, loop condition), D_8009FA60 (cleared), D_8009F9FC (8),
 * D_8009FAA4/D_8009FA30 (frame time base), D_8009FA50 (frame counter),
 * D_8009F284 (global tick).
 *
 * @par Callers
 * func_800287F4, on the path where arg1 != 0 (UI mode).
 */
void func_80027EE4(void)
{
    s32 t;
    gPaletteCycleCounter = 0;
    t = gPaletteCycleCounter;
    gFrameId = 8;
    D_8009FAA4 = t;
    D_8009FA30 = t;
    gSceneFrameCounter = 0;

    while (gScreenState == 4) {
        gSceneFrameCounter++;
        D_8009F284++;
        func_80027E68();
        func_800451EC(0);
        func_800462D0();
        func_80078DD0();
        func_80027EBC();
    }
}
