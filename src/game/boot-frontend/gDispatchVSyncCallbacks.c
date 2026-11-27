// Categories: Game Engine > Dispatch
#include "game/types.h"

extern volatile s32 gVSyncTickCounter __asm__("D_8009DD50");
extern  void (*gVSyncCallbackTable[8])(void) __asm__("D_8009DD30");
void gDispatchVSyncCallbacks(void) __asm__("func_80091BF8");


/**
 *  @brief Increments a counter and invokes registered callbacks.
 *
 *  Walks the eight-entry callback table in order, calling each non-null
 *  function after incrementing the volatile counter.
 *  @par Globals
 *  D_8009DD50 is incremented; D_8009DD30 supplies the callback table.
 *  @par Control flow
 *  Loops over eight entries and dispatches only non-null callbacks.
 *  @par Calls
 *  Indirectly calls each non-null function in D_8009DD30.
 *  @par Side effects
 *  Updates the counter; callbacks may have additional effects.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void gDispatchVSyncCallbacks(void)
{
    register void (**p)(void) __asm__("$16");
    s32 i;

    gVSyncTickCounter = gVSyncTickCounter + 1;
    for (i = 0, p = gVSyncCallbackTable; i < 8; i++) {
        if (*p != 0) {
            (*p)();
        }
        p++;
    }
}
