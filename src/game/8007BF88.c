// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009F8C8;
extern u8 *D_8009F8C4;
extern u8 D_80012094[];

/**
 *  @brief Selects a fixed global state and data pointer for argument 3.
 *
 *  When the argument is 3, stores 5 in the state and selects a data array.
 *  Other argument values leave both globals unchanged.
 *  @param arg0 Selector tested against 3.
 *  @par Globals
 *  D_8009F8C8 receives 5; D_8009F8C4 receives the address of D_80012094.
 *  @par Control flow
 *  A single conditional guards both assignments.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May update the global state and pointer.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007BF88(s32 arg0)
{
    if (arg0 == 3) {
        D_8009F8C8 = 5;
        D_8009F8C4 = D_80012094;
    }
}
