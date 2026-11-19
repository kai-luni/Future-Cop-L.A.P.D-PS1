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
 * @brief Forwards the player index stored in a state block to the input
 *        subsystem selector.
 *
 * @param state Outer state; +0xAC the inner block whose +0xC1 byte is the
 *        player index. Null selects index 0.
 * @return Nothing.
 *
 * @par Confirmed
 * index = state ? state->inner->index : 0; func_8001EA5C(index) runs.
 *
 * @par Likely role
 * The camera rig's player-context switch (func_800569EC): before polling
 * input for the followed unit, the input subsystem is pointed at that
 * unit's player slot.
 *
 * @par Unknown
 * What func_8001EA5C switches beyond the index.
 *
 * @par Callers
 * func_800569EC.
 *
 * @par Role and path
 * Resolves a controller port index from state->inner->index (+0xAC -> +0xC1,
 * defaulting to 0 when state is null) and forwards to func_8001EA5C to set
 * the large vibration motor level. Touches no globals directly; called by
 * func_800569EC.
 */
void gSetEntityLargeRumble(struct OuterState *state)
{
    int index = state != 0 ? state->inner->index : 0;
    gSetLargeMotor(index);
}
