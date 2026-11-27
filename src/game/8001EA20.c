// Categories: Game Engine > Input > Player Select
#include "../../include/game/types.h"

struct InnerState {
    u8 pad[0xC1];
    s8 index;
};

struct OuterState {
    u8 pad[0xAC];
    struct InnerState *inner;
};

extern void gSetLargeMotor(int index) __asm__("func_8001EA5C");
void gSetEntityLargeRumble(struct OuterState *state) __asm__("func_8001EA20");


/**
 * @brief Forwards the controller port index stored in an entity state block
 *        to the large vibration motor handler.
 *
 * @param state Outer state; +0xAC points to the inner block whose signed +0xC1
 *        byte supplies the port index. Null selects index 0.
 * @return Nothing.
 *
 * @par Confirmed
 * Computes `index = state != 0 ? state->inner->index : 0` and invokes
 * `gSetLargeMotor(index)` (`func_8001EA5C`). Touches no globals directly.
 *
 * @par Callers
 * func_800569EC.
 */
void gSetEntityLargeRumble(struct OuterState *state)
{
    int index = state != 0 ? state->inner->index : 0;
    gSetLargeMotor(index);
}
