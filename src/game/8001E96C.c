// Categories: Input > Vibration | Input > Controller
#include "game/types.h"

extern void func_8001E9AC(s32 port, s32 enable);

/**
 * @brief Clamps a controller port index to [0, 1] and toggles its small actuator.
 *
 * Clamps signed port values below 0 to 0 and values >= 2 to 1, then delegates (clamped, enable)
 * to func_8001E9AC to activate or stop the small vibration motor on D_800A6FF8.
 *
 * @param port Controller port index (clamped to 0 or 1).
 * @param enable Non-zero to start the small motor for 300 ticks, or 0 to stop it.
 *
 * @par Globals
 * None directly; func_8001E9AC updates D_800A6FF8[clamped].
 *
 * @par Control flow
 * Two nested conditionals clamping port into [0, 1], followed by calling func_8001E9AC.
 *
 * @par Calls
 * func_8001E9AC - configures the small vibration actuator and timer for the clamped port.
 *
 * @par Side effects
 * Updates D_800A6FF8[clamped] motor/timer state via func_8001E9AC.
 *
 * @par Callers
 * Confirmed: func_8007C940 (0x8007C9FC, triggers a single shake on vibration toggle enable) / Likely: none / Unknown: none.
 */
void func_8001E96C(s32 port, s32 enable)
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
    func_8001E9AC(clamped, enable);
}
