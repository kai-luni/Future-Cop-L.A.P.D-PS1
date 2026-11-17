// Categories: Game Engine > Frame | Graphics > HUD
#include "game/types.h"

extern s32 gPlayerCount __asm__("D_8009FA0C");
extern void func_800486FC(s32 arg);

/**
 * @brief Runs the single-player frame-close HUD refresh via func_800486FC when D_8009FA0C equals 1.
 *
 * Confirmed: checks D_8009FA0C == 1 and, when true, invokes func_800486FC(1); otherwise returns immediately. Cross-referencing func_80028618 and func_800486FC shows D_8009FA0C == 1 corresponds to single-player mode and enable = 1 permits func_800485AC(0) status reset when D_8009FAB8 == 2.
 * Category follows per-frame gameplay loop caller func_80028618 and HUD/overlay refresh callee func_800486FC.
 * Likely: gates the single-player HUD/name/pause-overlay close pass after world rendering each frame; Unknown: exact semantics of D_8009FAB8 == 2 in func_800486FC.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA0C - active player/mode count tested for == 1.
 * @par Control flow
 * Single equality check on D_8009FA0C == 1 guarding a call to func_800486FC(1).
 * @par Calls
 * func_800486FC - updates pause overlay, resolves D_8009F9B4 name into D_8009FAC8, calls func_800487CC(0), and conditionally calls func_800485AC(0).
 * @par Side effects
 * None directly; side effects are delegated to func_800486FC when D_8009FA0C == 1.
 * @par Callers
 * Confirmed: func_80028618 (0x80028778 in asm/us/main.s, every frame during states 4, 8, and 0xD) / Likely: none / Unknown: none.
 */
void func_80027D70(void)
{
    if (gPlayerCount == 1) {
        func_800486FC(1);
    }
}
