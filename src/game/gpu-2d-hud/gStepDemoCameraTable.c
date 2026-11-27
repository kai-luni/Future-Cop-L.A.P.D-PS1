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

extern u8 gDemoCameraStates[][8] __asm__("D_800A2290");
u8 gStepDemoCameraTable(struct Unk56994 *state, s32 elapsed) __asm__("func_80056994");


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
 *
 * @par Environment path
 * Counts the s16 timer at +0x2FE of the camera info block down by the frame
 * delta; when it goes negative it is reset to 3000 and the step byte at
 * +0x313 is incremented (never wrapped here), and the function returns
 * D_800A2290[side][step] with side from +0xC1. gUpdateChaseCamera
 * (func_800569EC) calls it every frame while gDemoPlayback > 0 and uses the
 * returned byte as its camera state for that frame (800569EC.c:287). The
 * table contains random values 7..14 written by func_80056924, which
 * func_80024A24 calls at 80024A24.c:193.
 */
u8 gStepDemoCameraTable(struct Unk56994 *state, s32 elapsed)
{
    state->timer -= elapsed;
    if (state->timer < 0) {
        state->timer = 3000;
        state->step++;
    }
    return gDemoCameraStates[state->side][state->step];
}
