// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/command_state.h"

/**
 * @brief Queues DualShock escape/config-mode command 0x43 with a 1-byte argument on a CommandState.
 *
 * Confirmed: sets state->current = 0x43 (+0x36), state->value = state->inline_data (+0x2C -> +0x24), state->inline_data[0] = value (+0x24), and state->active = 1 (+0x35).
 * Category follows PS1 pad command 0x43 (enter/exit configuration mode) and CommandState packet layout read by func_8008C07C and dispatched from func_8008C444.
 * Likely: PSX pad library command builder toggling config mode (1 = enter, 0 = exit); Unknown: none.
 *
 * @param state Pointer to the port CommandState structure (+0x24, +0x2C, +0x35, +0x36 written).
 * @param value Mode byte stored in inline_data[0] (1 on step 1 to enter config mode; 0 on step 0xFE or 0xF3 exit).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional leaf setter; writes four fields in CommandState and returns.
 * @par Calls
 * None.
 * @par Side effects
 * Arms state to transmit 1-byte parameter command 0x43 on the next pad poll cycle.
 * @par Callers
 * Confirmed: func_8008C444 (0x8008C4C0 for step 1 with value 1; 0x8008C4D0 for step 0xFE or response[0]==0xF3 && field_E8==0 with value 0) / Likely: none / Unknown: none.
 */
void func_8008B71C(struct CommandState *state, u8 value)
{
    state->current = 0x43;
    state->value = state->inline_data;
    state->inline_data[0] = value;
    state->active = 1;
}
