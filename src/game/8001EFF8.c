// Categories: Game Engine > Frame | Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern void (*D_8009F99C)(void);
extern s32 D_8009F9CC;
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 D_8009FA60;
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern volatile u8 D_8009FA3C;

extern s32 func_80092C8C(s32 arg0);
extern void func_8001BFE4(void);
extern void func_8001BFA8(void);

/**
 * @brief Per-frame tick installed as a callback: runs the chained hook, advances
 *        the palette/timer counters and finishes a pending display transfer.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Calls the function pointer D_8009F99C when non-null. In screen modes 4 and
 * 13 (D_8009FA00) it adds 5 to D_8009FA60, the palette-cycle counter the HUD
 * reads as (D_8009FA60 >> 4) & 7; D_8009FA2C always gains 5. When the pending
 * flag D_8009FA3C is set, func_80092C8C(1) polls the GPU; on completion the
 * flag is cleared and func_8001BFE4 plus func_8001BFA8 run (fade-layer and
 * display-setup finish). Finally D_8009F9CC toggles between 0 and 1.
 *
 * @par Likely role
 * The vertical-blank/frame callback of the render driver: the +5 steps are
 * palette animation in the pause screens and a frame timer, the toggle is
 * the double-buffer page flag.
 *
 * @par Unknown
 * What the chained hook D_8009F99C points at in each screen mode.
 *
 * @par Callers
 * No direct callers in the call graph. func_8001F0B8 installs the callback
 * table at D_8001EFF8 through func_80091510, so the interrupt path reaches
 * it indirectly.
 */
void func_8001EFF8(void)
{
    if (D_8009F99C != 0) {
        D_8009F99C();
    }
    if (gScreenState == 4 || gScreenState == 13) {
        D_8009FA60 += 5;
    }
    gFrameTickCounter += 5;
    if (D_8009FA3C != 0) {
        if (func_80092C8C(1) == 0) {
            D_8009FA3C = 0;
            func_8001BFE4();
            func_8001BFA8();
        }
    }
    D_8009F9CC = 1 - D_8009F9CC;
}
