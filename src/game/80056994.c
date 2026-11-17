// Categories: Graphics > Camera > State Machine
#include "game/types.h"

struct Unk56994 {
    u8 unk00[0xC1];
    s8 side;
    u8 unkC2[0x23C];
    s16 timer;
    u8 unk300[0x13];
    u8 step;
};

extern u8 gSideStepTable[][8] __asm__("D_800A2290");

/**
 * @brief Advances the camera preset phase timer and returns the current
 *        phase byte from the per-side table.
 *
 * @param state Camera state; +0xC1 the side, +0x2FE the timer,
 *        +0x313 the step.
 * @param elapsed Frame delta.
 * @return D_800A2290[side][step].
 *
 * @par Confirmed
 * timer -= elapsed; when it drops below 0 it resets to 3000 and step
 * increments (no wrap here). The table lookup uses side and step.
 *
 * @par Likely role
 * The input-phase clock of the chase camera func_800569EC: every 3000
 * time units the rig advances to the next preset phase, the table encodes
 * the allowed transitions per side.
 *
 * @par Unknown
 * The table dimensions and what wraps the step.
 *
 * @par Callers
 * func_800569EC.
 */
u8 func_80056994(struct Unk56994 *state, s32 elapsed)
{
    state->timer -= elapsed;
    if (state->timer < 0) {
        state->timer = 3000;
        state->step++;
    }
    return gSideStepTable[state->side][state->step];
}
