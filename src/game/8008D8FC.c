// Categories: Game Engine > Initialization
#include "game/types.h"

extern void func_8008DAD0(void);
extern void func_8008D7B0(s32 value);
extern void func_8008DB00(void);
s32 gStartBiosPadPolling(void) __asm__("func_8008D8FC");


/**
 *  @brief Runs three routines in sequence and reports success.
 *
 *  Calls the first routine, passes zero to the second, and calls the third.
 *  The wrapper returns a fixed result regardless of their effects.
 *  @return Always 1.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Executes all three calls unconditionally in source order.
 *  @par Calls
 *  func_8008DAD0 runs first; func_8008D7B0 receives zero;
 *  func_8008DB00 runs last.
 *  @par Side effects
 *  Any effects are those of the called routines.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Executes func_8008DAD0 (BIOS B0:13), func_8008D7B0(0) (BIOS B0:5B), and
 * func_8008DB00 (computed jump to jtbl_800A3E18_main) in sequence and always
 * returns 1. Accesses no globals directly and has no direct callers in src/.
 */
s32 gStartBiosPadPolling(void)
{
    func_8008DAD0();
    func_8008D7B0(0);
    func_8008DB00();
    return 1;
}
