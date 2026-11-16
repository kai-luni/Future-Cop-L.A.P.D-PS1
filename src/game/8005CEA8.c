// Categories: Gameplay > Effect State | Graphics > Effects
#include "../../include/game/types.h"

struct MotionState {
    s32 x;
    s32 y;
    s32 scale;
    s16 angle;
    u8 pad_0E[2];
    s16 velocity;
    u8 pad_12[2];
    s32 timer;
    u8 enabled;
    u8 state;
};

/**
 * @brief Resets one motion/animation state block to its neutral values.
 *
 * The block is put into a usable idle state: enabled flag set, timer, angle,
 * velocity and state byte cleared, both offsets zeroed and the scale set to
 * 1.0 in 16.16 fixed point (0x10000). Pointer fields are untouched, so the
 * caller keeps whatever it set up before.
 *
 * @param state Motion state block (x 0x00, y 0x04, scale 0x08, angle 0x0C,
 *              velocity 0x10, timer 0x14, enabled 0x18, state 0x19).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line stores, no branches.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites the whole state block.
 *
 * @par Callers
 * Unknown: no direct edge; the layout matches the HUD/effect motion records of
 * the same address range.
 */
void func_8005CEA8(struct MotionState *state)
{
    state->enabled = 1;
    state->timer = 0;
    state->angle = 0;
    state->velocity = 0;
    state->state = 0;
    state->x = 0;
    state->y = 0;
    state->scale = 0x10000;
}
