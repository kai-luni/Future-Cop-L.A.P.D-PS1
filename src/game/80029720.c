// Categories: Game Engine > State
#include "game/types.h"

extern u8 gBitFieldState __asm__("D_8009F6B8");

/**
 *  @brief Clears a state flag and conditionally sets another.
 *
 *  Clears bit 0x10 in the global state byte.
 *  If the original byte lacks bit 0x20, it also sets bit 0x08.
 *  @par Globals
 *  D_8009F6B8 is read and updated as a bit field.
 *  @par Control flow
 *  Tests bit 0x20 from the original value after clearing bit 0x10.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates D_8009F6B8; other bits retain their original values.
 *  @par Callers
 *  Confirmed: func_80023E84.
 */
void func_80029720(void)
{
    u8 state = gBitFieldState;

    gBitFieldState = state & 0xEF;
    if ((state & 0x20) == 0) {
        gBitFieldState = (state & 0xEF) | 0x08;
    }
}
