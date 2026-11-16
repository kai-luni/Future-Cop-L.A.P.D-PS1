// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void *jtbl_800A3E1C_main;

/**
 *  @brief Transfers control to the stored dispatch target.
 *
 *  Loads the target from jtbl_800A3E1C_main and jumps to it directly,
 *  allowing the caller to enter the selected target through this stub.
 *  @par Globals
 *  jtbl_800A3E1C_main supplies the jump target.
 *  @par Control flow
 *  Uses an unconditional computed goto with no local return path.
 *  @par Calls
 *  No named functions are called; control passes to the stored target.
 *  @par Side effects
 *  Any further effects depend on the target; the stub does not write memory.
 *  @par Callers
 *  Confirmed: func_8008D92C.
 */
void func_8008DB14(void)
{
    register void *target asm("$9") = jtbl_800A3E1C_main;

    goto *target;
}
