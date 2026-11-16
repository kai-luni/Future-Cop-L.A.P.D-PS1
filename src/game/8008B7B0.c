// Categories: Game Engine > Command State
#include "../../include/game/command_state.h"

/**
 *  @brief Initializes a command state with command code 0x4B.
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
 *  Confirmed: func_8008B168.
 */
void func_8008B7B0(struct CommandState *state)
{
    state->current = 0x4B;
    state->value = 0;
    state->active = 0;
}
