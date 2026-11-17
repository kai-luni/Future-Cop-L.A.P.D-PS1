// Categories: Graphics > HUD | Game Engine > State Machine
#include "game/types.h"

extern s32 gScreenState[] __asm__("D_8009FA00");
extern s32 D_8009FAB8;
extern s32 gOverlayTimer __asm__("D_8009F760");
extern s32 D_8009FAC8;

extern u8 func_80048790(void);

/**
 * @brief Updates the HUD layout mode D_8009FAB8 on overlay transitions and optional manual cycle input.
 *
 * Detects rising edges of D_8009FAC8 (versus cached D_8009F760) to force mode 2; otherwise
 * advances D_8009FAB8 modulo 3 while func_80048790() rejects mode 2 when D_8009F760 == 0
 * or when the low byte of input requests a cycle outside screen mode D_8009FA00[0] == 5.
 *
 * @param input Cycle request flag (low byte tested; suppressed if D_8009FA00[0] == 5 or on D_8009FAC8 rising edge).
 *
 * @par Globals
 * D_8009FA00 - screen mode checked against 5; D_8009FAC8 - current overlay state;
 * D_8009F760 - previous overlay state updated here; D_8009FAB8 - HUD layout mode (0..2) cycled/updated.
 * @par Control flow
 * Mode-5 guard, rising-edge check on D_8009FAC8, then conditional do-while loop incrementing D_8009FAB8 mod 3 while func_80048790().
 * @par Calls
 * func_80048790 - returns 1 when D_8009FAB8 == 2 and D_8009F760 == 0, preventing mode 2 without an active overlay.
 * @par Side effects
 * Updates D_8009F760 and D_8009FAB8.
 * @par Callers
 * Confirmed: func_800486FC (per-frame HUD close hook, passed 0).
 */
void func_800487CC(s32 input)
{
    register s32 cycle __asm__("$16");
    register s32 force __asm__("$17");
    s32 current;

    force = input;
    if ((u8)force && gScreenState[0] == 5) {
        force = 0;
    }
    current = D_8009FAC8;
    if (current != 0 && gOverlayTimer != current) {
        D_8009FAB8 = 2;
        force = 0;
    }
    gOverlayTimer = current;
    cycle = 0;
    if (func_80048790() || (u8)force) {
        cycle = 1;
    }
    if ((u8)cycle) {
        do {
            D_8009FAB8++;
            if (D_8009FAB8 >= 3) {
                D_8009FAB8 = 0;
            }
        } while (func_80048790());
    }
}
