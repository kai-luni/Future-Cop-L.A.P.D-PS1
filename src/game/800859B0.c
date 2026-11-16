// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;
extern s32 D_8009F4CC;
extern s32 D_8009F508;

/**
 *  @brief Initializes five global state values.
 *
 *  Assigns fixed values to a group of globals whose specific roles are not
 *  established by this function.
 *  @par Globals
 *  D_8009F4C0 and D_8009F508 are set to 1; D_8009F4C4 is set to 0.
 *  D_8009F4C8 and D_8009F4CC are set to 3.
 *  @par Control flow
 *  Performs the assignments unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the five global values.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800859B0(void)
{
    D_8009F4C0 = 1;
    D_8009F4C8 = 3;
    D_8009F4CC = 3;
    D_8009F4C4 = 0;
    D_8009F508 = 1;
}
