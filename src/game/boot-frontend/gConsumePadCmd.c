// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"
int gConsumePadCmd(struct CommandState *state) __asm__("func_8008C06C");


/**
 *  @brief Moves the current command value to previous and clears current.
 *
 *  This operation consumes the current value in a CommandState while
 *  retaining it in the previous field and returning it to the caller.
 *  @param state Command state to update.
 *  @return The original current value.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Reads current, clears it, updates previous, and returns the saved value.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the current and previous fields of state.
 *  @par Callers
 *  Confirmed: func_8008BF78.
 *
 * @par Role and path
 * Reads state->current from the CommandState struct, clears state->current to
 * 0, stores the original value into state->previous, and returns it. Called
 * directly by func_8008BF78 and bound to D_8009C74C by func_8008BDB0 for
 * indirect dispatch.
 */
int gConsumePadCmd(struct CommandState *state)
{
    int current = state->current;
    state->current = 0;
    state->previous = current;
    return current;
}
