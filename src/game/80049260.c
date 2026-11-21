// Categories: Platform > Sound > SE | Graphics > Text
#include "game/types.h"

extern u32 gEffectId __asm__("D_8009FACC");

extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");

/**
 * @brief Consumes the pending text blip request and clears it.
 *
 * D_8009FACC holds the effect id the text engine wants played for the current screen; func_80069B1C hands its address, not its value, to the text state block at +0xF0, so the renderer stores the id there directly.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009FACC - pending effect id; read, then zeroed. func_80048EC4 clears it too on a text reset.
 *
 * @par Control flow
 * Zero test, then a range test against 0x82 that keeps the value inside the valid effect range, a conditional call and an unconditional clear; no loops.
 *
 * @par Calls
 * func_80046C84 - starts the effect the id names, with parameter 0.
 *
 * @par Side effects
 * At most one voice starts and D_8009FACC is zeroed, so the request is consumed exactly once per frame. func_800486FC calls this at frame close in screen mode 4.
 *
 * @par Callers
 * Confirmed: func_800486FC (0x8004871C), the frame-close HUD hook of screen mode 4.
 */
void func_80049260(void)
{
    if (gEffectId != 0) {
        if (gEffectId < 0x82) {
            gPlayNonpositionalSoundEffect(gEffectId, 0);
        }
        gEffectId = 0;
    }
}
