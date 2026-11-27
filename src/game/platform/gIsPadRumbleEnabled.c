// Categories: Platform > Controller Input | Input > Controller
#include "game/types.h"

extern u8 gPadActuators[] __asm__("D_800A6FF8");
u8 gIsPadRumbleEnabled(s32 arg0) __asm__("func_8001EB2C");


/**
 * @brief Reads the rumble-enable flag (byte +0x17) for a clamped controller port in D_800A6FF8.
 *
 * Clamps port index `arg0` to `[0, 1]` using 0x18-byte strides from D_800A6FF8 and returns the trailing `u8` field at +0x17 (`PadActuator.enabled` in func_8001E9AC/func_8001EC04). Category Platform > Controller Input | Input > Controller follows the D_800A6FF8 actuator table and pad callers.
 *
 * @param arg0 Controller port index (values < 0 clamp to slot 0; >= 2 clamp to slot 1).
 * @return Byte at offset +0x17 (`enabled` flag) for the selected D_800A6FF8 slot.
 *
 * @par Globals
 * D_800A6FF8 - two-slot array of 0x18-byte `PadActuator` records; reads byte +0x17 of slot 0 or 1.
 *
 * @par Control flow
 * Clamps `arg0` via nested signed comparisons (`arg0 >= 0`, `arg0 < 2`) to byte offset 0 or 0x18, then returns `(base + offset)[0x17]`.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8007C088 (0x8007C0A4, 0x8007C0B0), func_8007C940 (0x8007C9E0), func_80080A5C (0x80080A8C, 0x80080A98), and func_80078C88 (0x80078D84, 0x80078D90 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Clamps the controller port index to [0, 1] and returns the enabled flag
 * byte at offset +0x17 of that slot in gPadActuators (D_800A6FF8). Read-only
 * query with no calls; called by func_8007C088, func_8007C940, func_80080A5C,
 * and func_80078C88.
 */
u8 gIsPadRumbleEnabled(s32 arg0)
{
    u8 *base = gPadActuators;
    s32 offset;

    if (arg0 >= 0) {
        if (arg0 < 2) {
            offset = arg0 * 0x18;
        } else {
            offset = 0x18;
        }
    } else {
        offset = 0;
    }
    return (base + offset)[0x17];
}
