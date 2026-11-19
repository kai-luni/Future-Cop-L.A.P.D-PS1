// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void *jtbl_800A3E18_main;

/**
 *  @brief Transfers control to a stored dispatch target.
 *
 *  Loads the target from jtbl_800A3E18_main and jumps to it using
 *  a computed goto rather than selecting an entry in this function.
 *  @par Globals
 *  jtbl_800A3E18_main supplies the jump target.
 *  @par Control flow
 *  Unconditionally jumps to the loaded target.
 *  @par Calls
 *  None listed; control is transferred indirectly.
 *  @par Side effects
 *  Depend on the target; the target must be valid.
 *  @par Callers
 *  Confirmed: func_8008D8FC.
 *
 * @par Role and path
 * Loads the pointer from jtbl_800A3E18_main (populated by func_8008DB28 as
 * kernel base + 0x884) into $9 and performs an unconditional computed jump
 * without a call frame. Driven by func_8008D8FC.
 */
void func_8008DB00(void)
{
    register void *target __asm__("$9") = jtbl_800A3E18_main;
    goto *target;
}
