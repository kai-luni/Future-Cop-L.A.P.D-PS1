// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"

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
 */
int func_8008C06C(struct CommandState *state)
{
    int current = state->current;
    state->current = 0;
    state->previous = current;
    return current;
}
