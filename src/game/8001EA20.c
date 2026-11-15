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

extern void func_8001EA5C(int index);

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
 */
void func_8001EA20(struct OuterState *state)
{
    int index = state != 0 ? state->inner->index : 0;
    func_8001EA5C(index);
}
