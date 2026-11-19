// Categories: Input > DualShock | Input > Controller
#include "game/types.h"

extern u8 gPadActuators[] __asm__("D_800A6FF8");
u8 gIsPadRumbleReady(s32 arg0) __asm__("func_8001EAEC");


/**
 * @brief Returns the DualShock actuator/rumble-ready flag byte (+0x16) for a clamped controller slot.
 *
 * Confirmed: clamps arg0 to 0 (when arg0 <= 0) or 1 (when arg0 >= 1, computing byte offset 0 or 0x18 into 0x18-byte struct array D_800A6FF8), and returns *(u8 *)(D_800A6FF8 + offset + 0x16). Cross-referencing func_8001EC94 shows +0x16 is State.flag, cleared on disconnect and set nonzero once actuator slots are configured.
 * Category follows DualShock controller state table D_800A6FF8 managed by func_8001EC94 and front-end pad queries in func_8007C6F0/func_8007C940.
 * Likely: queries whether the DualShock vibration capability has been detected and initialized for controller slot 0 or 1; Unknown: whether callers rely on arg0 >= 2 clamping to slot 1.
 *
 * @param arg0 Controller slot index; negative values clamp to 0 and values >= 1 clamp to 1.
 * @return Value of D_800A6FF8[clamped].flag at offset +0x16 (0 if unavailable, nonzero when active).
 *
 * @par Globals
 * D_800A6FF8 - 2-entry array of 0x18-byte controller actuator State structs (offset +0x16 read).
 * @par Control flow
 * Clamps arg0 into byte offset 0 or 0x18 via nested comparisons (arg0 >= 0, arg0 < 2), then loads byte +0x16.
 * @par Calls
 * None.
 * @par Side effects
 * None observed (read-only query).
 * @par Callers
 * Confirmed: func_8007C6F0 (0x8007C798, gates vibration prompt icon); func_8007C940 (0x8007C9AC, gates rumble toggle test) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Clamps the controller port index to [0, 1] and returns the byte at offset
 * +0x16 of that slot in gPadActuators (D_800A6FF8), indicating whether
 * DualShock actuator capability is active. Read-only query with no calls;
 * called by func_8007C6F0 and func_8007C940.
 */
u8 gIsPadRumbleReady(s32 arg0)
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

    return *(base + offset + 0x16);
}
