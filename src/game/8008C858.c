// Categories: Platform > Device State
#include "game/types.h"

typedef struct {
    u8 pad_00[0x46];
    u8 device;
    u8 pad_47[0x9F];
    u16 ready;
} Unk8008C858;

/**
 *  @brief Tests a device state using its ready and device fields.
 *
 *  Returns zero only when the ready field is nonzero and the device byte is 0xFF.
 *  All other combinations produce a nonzero result.
 *  @param state State containing the device byte and ready field.
 *  @return 0 for a nonzero ready field with device 0xFF, otherwise 1.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Checks ready first and reads device only when ready is nonzero.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8008C518.
 */
s32 func_8008C858(Unk8008C858 *state)
{
    register u32 tmp __asm__("$2");
    register u32 device __asm__("$3");

    tmp = state->ready;
    if (tmp == 0) {
        return 1;
    }
    tmp = 0xFF;
    device = state->device;
    if (device == tmp) {
        return 0;
    }
    return 1;
}