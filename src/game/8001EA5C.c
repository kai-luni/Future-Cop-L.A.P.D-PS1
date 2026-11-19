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
void gSetLargeMotor(s32 port, s32 level) __asm__("func_8001EA5C");


/**
 *  @brief Sets the large motor level for a controller port.
 *
 *  The actuator must be enabled and have a valid large-motor index.
 *  A nonzero level sets a deadline 300 units after the current global time.
 *  @param port Index of the actuator record.
 *  @param level Motor level, clamped to the range 0 through 255.
 *  @par Globals
 *  D_800A6FF8 holds actuator records; D_8009FA2C supplies the current time.
 *  @par Control flow
 *  Disabled or unmapped actuators return; the clamped level selects the update.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the selected motor byte and large-motor timer.
 *  @par Callers
 *  Confirmed: func_8001EA20, func_8001EBA8.
 *
 * @par Role and path
 * Clamps level to [0, 255] and updates the large vibration motor state on
 * gPadActuators[port] when enabled and largeIndex is not 0xFF. Non-zero
 * levels write motor[largeIndex] and set largeTimer to gFrameTickCounter +
 * 300; zero clears both. Reads D_8009FA2C and updates D_800A6FF8; called by
 * func_8001EA20 and func_8001EBA8.
 */
void gSetLargeMotor(s32 port, s32 level)
{
    struct PadActuator *actuator;
    s32 clamped;
    s32 index;

    actuator = &gPadActuators[port];
    if (!actuator->enabled || (index = actuator->largeIndex) == 0xFF) {
        return;
    }

    clamped = 0xFF;
    if (level >= 0) {
        if (level < 0x100) {
            clamped = level;
        }
    } else {
        clamped = 0;
    }
    level = clamped;

    if (level) {
        actuator->motor[index] = level;
        actuator->largeTimer = gFrameTickCounter + 300;
    } else {
        actuator->motor[index] = 0;
        actuator->largeTimer = 0;
    }
}
