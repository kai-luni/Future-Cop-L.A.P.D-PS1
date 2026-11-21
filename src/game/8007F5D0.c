// Categories: Game Engine > State
#include "game/types.h"

extern u8 gArmedFlag __asm__("D_8009FAC4");
extern u8 gRowClearFlag __asm__("D_8009FA10");
extern u8 gScreenDescriptor[] __asm__("D_800A3838");
extern u8 gMenuDescriptor[] __asm__("D_800A3808");
extern void gActivateMenuPanel(u8 *, s32) __asm__("func_80078650");

/**
 *  @brief Selects one of two global buffers for a routine call.
 *
 *  Selects D_800A3838 only when D_8009FAC4 is zero and D_8009FA10 is nonzero.
 *  Otherwise it selects D_800A3808; both paths pass 1 as the second argument.
 *  @par Globals
 *  D_8009FAC4 and D_8009FA10 control selection; D_800A3838 and
 *  D_800A3808 are the candidate buffers.
 *  @par Control flow
 *  Branches on the two flags and makes one call on either path.
 *  @par Calls
 *  func_80078650 receives the selected buffer and 1.
 *  @par Side effects
 *  Any effects are those of func_80078650.
 *  @par Callers
 *  Confirmed: func_8007F618, func_8007F648.
 */
void func_8007F5D0(void)
{
    if (gArmedFlag == 0 && gRowClearFlag != 0) {
        gActivateMenuPanel(gScreenDescriptor, 1);
    } else {
        gActivateMenuPanel(gMenuDescriptor, 1);
    }
}
