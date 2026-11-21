// Categories: Game Engine > State
#include "game/types.h"

extern void gBiosAllowNewCard(s32) __asm__("func_8008D6D0");
extern void gBiosWriteCardSector(s32, s32, s32) __asm__("func_8008D670");
void gApplyBiosStateValue(s32 arg0) __asm__("func_8008D690");


/**
 *  @brief Calls two helpers in sequence for the supplied value.
 *
 *  The first helper receives the argument alone; the second receives it
 *  with the fixed values 0x3F and zero.
 *  @param arg0 Signed value passed to both helpers.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Calls the first helper and then the second without branching.
 *  @par Calls
 *  func_8008D6D0 receives arg0; func_8008D670 receives arg0, 0x3F, and 0.
 *  @par Side effects
 *  No direct writes; the helpers' effects are not specified here.
 *  @par Callers
 *  Confirmed: func_8001FD28.
 *
 * @par Role and path
 * Calls func_8008D6D0 with arg0 and then func_8008D670 with arg0, 0x3F, and 0
 * without branching. Reads no globals directly and is called by func_8001FD28
 * as a default state transition. Hands off to BIOS vector B0:50 via
 * func_8008D6D0 and BIOS vector B0:4E via func_8008D670.
 */
void gApplyBiosStateValue(s32 arg0) {
    gBiosAllowNewCard(arg0);
    gBiosWriteCardSector(arg0, 0x3F, 0);
}
