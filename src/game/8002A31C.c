// Categories: Game Engine > State
#include "game/types.h"

extern volatile s32 D_8009F6D8;
extern s32 D_800A0C60[];

extern void func_80029650(void);
void gQueueStreamAssetRelease(s32 arg0, s32 arg1) __asm__("func_8002A31C");


/**
 *  @brief Inserts a value into a four-entry indexed table when its slot is empty.
 *
 *  Uses the global index to select a slot; an occupied slot instead triggers
 *  a separate operation without inserting the value.
 *  @param arg0 Value to store in an empty slot.
 *  @param arg1 Unused.
 *  @par Globals
 *  D_800A0C60 is the slot table; D_8009F6D8 is its current index.
 *  @par Control flow
 *  On insertion, advances the index and wraps it to zero at four.
 *  @par Calls
 *  func_80029650 is called when the selected slot is nonzero.
 *  @par Side effects
 *  May write a table entry and update the volatile index; the call may have effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gQueueStreamAssetRelease(s32 arg0, s32 arg1)
{
    if (D_800A0C60[D_8009F6D8] != 0) {
        func_80029650();
    } else {
        D_800A0C60[D_8009F6D8] = arg0;
        D_8009F6D8 = D_8009F6D8 + 1;
        if (D_8009F6D8 >= 4) {
            D_8009F6D8 = 0;
        }
    }
}
