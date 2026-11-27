// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"
void gCmdPadActAlign(struct CommandState *state) __asm__("func_8008B4EC");


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
 *
 * @par Role and path
 * Sets state->current to 0x4D, state->active to 6, and state->value to
 * state->source. Installed as command_handler by func_8008B484 with no
 * globals or callees.
 */
void gCmdPadActAlign(struct CommandState *state)
{
    state->current = 0x4D;
    state->active = 6;
    state->value = state->source;
}
