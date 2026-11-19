// Categories: Input > Controller Setup | Platform > Controller Input
#include "game/types.h"

struct PadActuator {
    u8 port;
    u8 unk01;
    u8 motor[2];
    u8 smallIndex;
    u8 largeIndex;
    u8 unk06[2];
    s32 smallTimer;
    s32 largeTimer;
    u8 unk10[6];
    u8 mode;
    u8 enabled;
};

extern struct PadActuator gPadActuators[] __asm__("D_800A6FF8");

extern void gStopPadRumble(s32 port) __asm__("func_8001EBA8");
void gSetPadRumbleEnabled(s32 port, s32 enable) __asm__("func_8001EC04");


/**
 * @brief Enables or disables vibration actuator support for a controller port.
 *
 * Clamps port to [0, 1] and sets D_800A6FF8[port].enabled to 1 when enable is non-zero; when enable is 0, stops both motors via func_8001EBA8(port) before clearing enabled.
 *
 * @param port Controller port index (clamped to 0 or 1).
 * @param enable Non-zero enables actuators on port; 0 stops motors and disables them.
 *
 * @par Globals
 * D_800A6FF8 - PadActuator array; writes .enabled (+0x17) for the clamped port.
 *
 * @par Control flow
 * Clamps port to 0..1 using signed comparisons, then branches on enable != 0.
 *
 * @par Calls
 * func_8001EBA8 - stops both actuator motors on port when disabling.
 *
 * @par Side effects
 * Writes D_800A6FF8[port].enabled and, when disabling, clears motor outputs via func_8001EBA8.
 *
 * @par Callers
 * Confirmed: func_8007C80C, func_8007C940, func_80080984.
 *
 * @par Role and path
 * Clamps the controller port index to [0, 1] and sets
 * gPadActuators[port].enabled to 1 when enable is non-zero, or stops both
 * motors via func_8001EBA8(port) and clears enabled when zero. Updates
 * D_800A6FF8; called by func_8007C80C, func_8007C940, and func_80080984.
 */
void gSetPadRumbleEnabled(s32 port, s32 enable)
{
    s32 clamped;

    if (port >= 0) {
        if (port < 2) {
            clamped = port;
        } else {
            clamped = 1;
        }
    } else {
        clamped = 0;
    }
    port = clamped;
    if (enable) {
        gPadActuators[port].enabled = 1;
    } else {
        gStopPadRumble(port);
        gPadActuators[port].enabled = 0;
    }
}
