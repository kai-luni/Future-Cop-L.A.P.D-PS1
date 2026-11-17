// Categories: Input > Controller Vibration
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
    u8 unk16;
    u8 enabled;
};

extern struct PadActuator gPadActuators[] __asm__("D_800A6FF8");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");

/**
 *  @brief Sets the small motor state for a controller port.
 *
 *  The actuator record must be enabled and have a valid small-motor index.
 *  Enabling sets a deadline 300 units after the current global time.
 *  @param port Index of the actuator record.
 *  @param enable Nonzero to turn the motor on, zero to turn it off.
 *  @par Globals
 *  D_800A6FF8 holds actuator records; D_8009FA2C supplies the current time.
 *  @par Control flow
 *  Disabled or unmapped actuators return; otherwise enable selects the update.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the selected motor byte and small-motor timer.
 *  @par Callers
 *  Confirmed: func_8001E930, func_8001E96C, func_8001EBA8.
 */
void func_8001E9AC(s32 port, s32 enable)
{
    struct PadActuator *actuator;

    actuator = &gPadActuators[port];
    if (!actuator->enabled || actuator->smallIndex == 0xFF) {
        return;
    }
    if (enable) {
        actuator->motor[actuator->smallIndex] = 1;
        actuator->smallTimer = gFrameTickCounter + 300;
    } else {
        actuator->motor[actuator->smallIndex] = 0;
        actuator->smallTimer = 0;
    }
}
