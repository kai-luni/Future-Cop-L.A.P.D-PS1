// Categories: Game Engine > Mode State
#include "../../include/game/types.h"

struct ModeObject {
    u8 pad_00[8];
    s32 value;
};

struct ModeState {
    s32 mode;
    struct ModeObject *object;
    s32 value;
};

extern struct ModeState D_800A1800;

/**
 *  @brief Sets the current mode and optionally records an object and its value.
 *
 *  The mode is always stored in D_800A1800.
 *  Mode -1 leaves the previously stored object and value unchanged.
 *  @param mode Mode to store; -1 suppresses the object update.
 *  @param object Object to store and read when mode is not -1.
 *  @par Globals
 *  D_800A1800 holds the mode, object pointer, and copied object value.
 *  @par Control flow
 *  Branches on mode after storing it; a nonnegative mode is not required.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the mode and, unless mode is -1, the object pointer and value.
 *  @par Callers
 *  Confirmed: func_80029C34.
 */
void func_80044FF8(s32 mode, struct ModeObject *object)
{
    D_800A1800.mode = mode;
    if (mode != -1) {
        D_800A1800.object = object;
        D_800A1800.value = object->value;
    }
}
