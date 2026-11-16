// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"

/**
 *  @brief Sets a command state to command 0x4D with active value 6.
 *
 *  Copies the state's source field into its value field while setting
 *  the current and active fields to their fixed values.
 *  @param state Command state to update.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Performs three assignments without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the current, active, and value fields of state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008B4EC(struct CommandState *state)
{
    state->current = 0x4D;
    state->active = 6;
    state->value = state->source;
}
