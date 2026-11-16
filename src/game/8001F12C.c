// Categories: Game Engine > Callbacks
#include "game/types.h"

extern s32 D_8009F99C;

/**
 *  @brief Clears the registered callback value if it matches the argument.
 *
 *  Compares the supplied value with the single global callback slot.
 *  An empty slot or a mismatch is reported without changing the slot.
 *  @param callback Callback value to match against the registered value.
 *  @return 0 if the matching slot was cleared, or 3 otherwise.
 *  @par Globals
 *  D_8009F99C holds the registered callback value and is cleared on a match.
 *  @par Control flow
 *  Checks for an empty slot, then tests for a match.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May set D_8009F99C to zero.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8001F12C(s32 callback)
{
    if (D_8009F99C == 0) {
        return 3;
    }
    if (callback == D_8009F99C) {
        D_8009F99C = 0;
        return 0;
    }
    return 3;
}
