// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"
void gSetPadModelQueryCmd(struct CommandState *state) __asm__("func_8008B73C");


/**
 *  @brief Initializes a command state with command code 0x45.
 *
 *  Sets the current command and clears its value and active fields,
 *  providing a defined state for the caller's command processing.
 *  @param state Command state to initialize.
 *  @par Globals
 *  No named globals are referenced.
 *  @par Control flow
 *  Performs three assignments without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the current, value, and active fields of state.
 *  @par Callers
 *  Confirmed: func_8008AE8C.
 *
 * @par Role and path
 * Sets state->current to 0x45, state->value to 0, and state->active to 0.
 * Called by func_8008AE8C at sequence_step 2.
 */
void gSetPadModelQueryCmd(struct CommandState *state)
{
    state->current = 0x45;
    state->value = 0;
    state->active = 0;
}
