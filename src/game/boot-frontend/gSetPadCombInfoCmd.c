// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/command_state.h"
void gSetPadCombInfoCmd(struct CommandState *state, u8 value) __asm__("func_8008B790");


/**
 * @brief Queues DualShock command 0x47 (combination record query) with a 1-byte index on a CommandState.
 *
 * Confirmed: sets state->current = 0x47 (+0x36), state->value = state->inline_data (+0x2C -> +0x24), state->inline_data[0] = value (+0x24), and state->active = 1 (+0x35).
 * Category follows PS1 DualShock config command 0x47 dispatched at sequence_step 4 in func_8008AE8C and func_8008B168 and parsed into PadCombinationRecord by func_8008B210.
 * Likely: PSX pad library command builder initiating transfer of an actuator combination descriptor by info_record_index; Unknown: none.
 *
 * @param state Pointer to the port CommandState structure (+0x24, +0x2C, +0x35, +0x36 written).
 * @param value Combination record index stored into inline_data[0] (port->info_record_index).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional leaf setter; writes four fields in CommandState and returns.
 * @par Calls
 * None.
 * @par Side effects
 * Arms state to transmit 1-byte parameter command 0x47 on the next pad poll cycle.
 * @par Callers
 * Confirmed: func_8008AE8C (0x8008AEF8, sequence_step 4); func_8008B168 (0x8008B1E8, sequence_step 4 when combination_bytes_remaining == 0) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Sets state->current to 0x47, state->value to state->inline_data,
 * state->inline_data[0] to value, and state->active to 1. Called by
 * func_8008AE8C and func_8008B168 at sequence_step 4.
 */
void gSetPadCombInfoCmd(struct CommandState *state, u8 value)
{
    state->current = 0x47;
    state->value = state->inline_data;
    state->inline_data[0] = value;
    state->active = 1;
}
