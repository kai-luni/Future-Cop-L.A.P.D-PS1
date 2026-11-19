// Categories: Platform > Controller | Input > DualShock
#include "game/types.h"

extern void gSetSmallMotor(s32 port, s32 enable) __asm__("func_8001E9AC");
extern void gSetLargeMotor(s32 port, s32 level) __asm__("func_8001EA5C");
void gStopPadRumble(s32 port) __asm__("func_8001EBA8");


/**
 * @brief Switches a pad port's motors and mode off.
 *
 * The port is clamped to 0..1 (negative becomes 0, anything above 1 becomes 1),
 * then both the enable path (func_8001E9AC) and the motor level path
 * (func_8001EA5C) are called with 0, which stops vibration and clears the mode.
 * @param port Requested pad port.
 * @par Globals
 * None.
 * @par Control flow
 * Clamp, then two calls.
 * @par Calls
 * func_8001E9AC - motor enable. func_8001EA5C - motor level.
 * @par Side effects
 * Stops the pad's vibration.
 * @par Callers
 * Unknown: no direct edge; the counterpart of the motor start path.
 *
 * @par Role and path
 * Clamps the controller port index to [0, 1] and stops both vibration
 * actuators by calling func_8001E9AC(port, 0) and func_8001EA5C(port, 0).
 * Touches no globals directly; called by func_8001EB6C and func_8001EC04.
 */
void gStopPadRumble(s32 port)
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
    gSetSmallMotor(port, 0);
    gSetLargeMotor(port, 0);
}
