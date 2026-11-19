// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/command_state.h"
void gSetPadModeConstCmd(struct CommandState *state, u8 value) __asm__("func_8008B750");


/**
 * @brief Queues DualShock command 0x4C (mode/constant query) with a 1-byte index on a CommandState.
 *
 * Confirmed: sets state->current = 0x4C (+0x36), state->value = state->inline_data (+0x2C -> +0x24), state->inline_data[0] = value (+0x24), and state->active = 1 (+0x35).
 * Category follows PS1 DualShock config command 0x4C used during step 2 of func_8008B168/func_8008B210 (mode_table) and step 3 of func_8008AE8C.
 * Likely: PSX pad library command builder requesting a mode-table entry or current mode descriptor by index; Unknown: exact meaning of reply in func_8008AE8C step 3.
 *
 * @param state Pointer to the port CommandState structure (+0x24, +0x2C, +0x35, +0x36 written).
 * @param value Query index byte stored into inline_data[0] (info_record_index or current_mode_index).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional leaf setter; writes four fields in CommandState and returns.
 * @par Calls
 * None.
 * @par Side effects
 * Arms state to transmit 1-byte parameter command 0x4C on the next pad poll cycle.
 * @par Callers
 * Confirmed: func_8008AE8C (0x8008AEE4 with port->current_mode_index); func_8008B168 (0x8008B1B0 with port->info_record_index) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Sets state->current to 0x4C, state->value to state->inline_data,
 * state->inline_data[0] to value, and state->active to 1. Called by
 * func_8008AE8C and func_8008B168.
 */
void gSetPadModeConstCmd(struct CommandState *state, u8 value)
{
    state->current = 0x4C;
    state->value = state->inline_data;
    state->inline_data[0] = value;
    state->active = 1;
}
