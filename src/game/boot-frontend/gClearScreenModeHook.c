// Categories: Game Engine > Callbacks
#include "game/types.h"

extern s32 gScreenModeHook __asm__("D_8009F99C");
s32 gClearScreenModeHook(s32 callback) __asm__("func_8001F12C");


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
 *
 * @par Role and path
 * Clears gScreenModeHook (D_8009F99C) to 0 and returns 0 if it is non-zero
 * and equals callback, otherwise leaving it unchanged and returning 3. Reads
 * and writes D_8009F99C and makes no calls; no direct callers are present in
 * the C tree.
 */
s32 gClearScreenModeHook(s32 callback)
{
    if (gScreenModeHook == 0) {
        return 3;
    }
    if (callback == gScreenModeHook) {
        gScreenModeHook = 0;
        return 0;
    }
    return 3;
}
