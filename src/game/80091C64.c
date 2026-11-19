// Categories: Game Engine > State Tables
#include "../../include/game/types.h"

extern s32 gVSyncCallbackTable[] __asm__("D_8009DD30");

/**
 *  @brief Updates an indexed state value when it differs from the current value.
 *
 *  Compares the supplied value with an entry in D_8009DD30.
 *  The store is skipped when the entry already matches.
 *  @param index Index of the table entry to update.
 *  @param value New signed value for the entry.
 *  @par Globals
 *  D_8009DD30 is read and conditionally written at index.
 *  @par Control flow
 *  Branches on the comparison; no index bounds check is performed.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May replace one table entry.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80091C64(u32 index, s32 value)
{
    if (value != gVSyncCallbackTable[index]) {
        gVSyncCallbackTable[index] = value;
    }
}
