// Categories: Input > DualShock | Platform > Controller
#include "../../include/game/command_state.h"
void gBindActuatorBuffer(struct CommandState *state, int value, u8 type) __asm__("func_8008AE70");


/**
 * @brief Binds a rumble actuator data buffer and byte count to a controller port command state.
 *
 * Confirmed: writes value to *(int *)&state->inline_data[4] (offset +0x28) and type to state->type (offset +0x34) in struct CommandState (command_state.h).
 * Category follows CommandState offsets read by func_8008C07C (data0/+0x28 and count0/+0x34) and DualShock setup in func_8001EC94 via func_8008A240.
 * Likely: PSX pad library PadSetAct backend configuring per-port actuator buffer and length; Unknown: full meaning of type values beyond byte count.
 *
 * @param state Pointer to the port CommandState structure (+0x28 and +0x34 written).
 * @param value Pointer (cast to int) to the caller actuator output byte buffer.
 * @param type Actuator byte count stored into state->type (+0x34).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional two-store leaf function; no branches or loops.
 * @par Calls
 * None.
 * @par Side effects
 * Mutates state->inline_data[4..7] (+0x28) and state->type (+0x34) on the supplied CommandState.
 * @par Callers
 * Confirmed: func_8008A240 (0x8008A26C in asm/us/main.s, called by func_8001EC94 with D_8009C73C(port), s->values, 2) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Stores value into *(int *)&state->inline_data[4] (offset +0x28) and type
 * into state->type (offset +0x34) on the supplied CommandState. Called by
 * func_8008A240 during DualShock rumble setup, with no globals or callees.
 */
void gBindActuatorBuffer(struct CommandState *state, int value, u8 type)
{
    *(int *)&state->inline_data[4] = value;
    state->type = type;
}
