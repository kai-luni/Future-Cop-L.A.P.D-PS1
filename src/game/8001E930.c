// Categories: Input > Controller | Input > Vibration
#include "../../include/game/types.h"

struct InnerState {
    u8 pad[0xC1];
    s8 index;
};

struct OuterState {
    u8 pad[0xAC];
    struct InnerState *inner;
};

extern void func_8001E9AC(int index);

/**
 * @brief Resolves a player entity's controller port index and toggles its small actuator.
 *
 * Extracts the signed port index from state->inner->index (+0xAC -> +0xC1), falling back to
 * port 0 when state is null, and calls func_8001E9AC with the caller-supplied enable flag in $a1.
 *
 * @param state Pointer to player/entity OuterState (inner at +0xAC, port byte at +0xC1), or 0 for port 0; $a1 enable flag passes through unchanged.
 *
 * @par Globals
 * None directly; func_8001E9AC updates D_800A6FF8[port].
 *
 * @par Control flow
 * Conditional selection of index (state != 0 ? state->inner->index : 0) followed by calling func_8001E9AC.
 *
 * @par Calls
 * func_8001E9AC - sets or clears the small vibration motor and 300-tick timer on D_800A6FF8[port].
 *
 * @par Side effects
 * Updates the controller actuator state in D_800A6FF8[index] via func_8001E9AC.
 *
 * @par Callers
 * Confirmed: func_80055988 (0x80055CDC, passes (obj, 1) during transformation and (obj, 0) on completion) / Likely: none / Unknown: none.
 */
void func_8001E930(struct OuterState *state)
{
    int index = state != 0 ? state->inner->index : 0;
    func_8001E9AC(index);
}
