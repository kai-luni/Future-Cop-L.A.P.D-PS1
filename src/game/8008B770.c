// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/command_state.h"

/**
 * @brief Queues DualShock command 0x46 (actuator capability query) with a 1-byte index on a CommandState.
 *
 * Confirmed: sets state->current = 0x46 (+0x36), state->value = state->inline_data (+0x2C -> +0x24), state->inline_data[0] = value (+0x24), and state->active = 1 (+0x35).
 * Category follows PS1 DualShock config command 0x46 dispatched at sequence_step 3 in func_8008B168 and parsed into PadCapabilityRecord by func_8008B210.
 * Likely: PSX pad library command builder querying actuator descriptor record by index during controller capability discovery; Unknown: none.
 *
 * @param state Pointer to the port CommandState structure (+0x24, +0x2C, +0x35, +0x36 written).
 * @param value Capability record index stored into inline_data[0] (port->info_record_index).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional leaf setter; writes four fields in CommandState and returns.
 * @par Calls
 * None.
 * @par Side effects
 * Arms state to transmit 1-byte parameter command 0x46 on the next pad poll cycle.
 * @par Callers
 * Confirmed: func_8008B168 (0x8008B1C4 in asm/us/main.s, sequence_step 3 with port->info_record_index) / Likely: none / Unknown: none.
 */
void func_8008B770(struct CommandState *state, u8 value)
{
    state->current = 0x46;
    state->value = state->inline_data;
    state->inline_data[0] = value;
    state->active = 1;
}
