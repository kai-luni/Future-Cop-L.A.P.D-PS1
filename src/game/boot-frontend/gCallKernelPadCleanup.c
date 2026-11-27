// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void *jtbl_800A3E1C_main;
void gCallKernelPadCleanup(void) __asm__("func_8008DB14");


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
 *
 * @par Role and path
 * Loads the pointer from jtbl_800A3E1C_main (populated by func_8008DB28 as
 * kernel base + 0x894) into $9 and performs an unconditional computed jump
 * without a call frame. Driven by func_8008D92C.
 */
void gCallKernelPadCleanup(void)
{
    register void *target asm("$9") = jtbl_800A3E1C_main;

    goto *target;
}
