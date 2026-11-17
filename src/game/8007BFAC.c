// Categories: Game Engine > State
#include "game/types.h"

extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern void *D_8009F8C4;
extern u8 D_80012088[];

/**
 *  @brief Selects a fixed state and data pointer when the argument is 3.
 *
 *  For the recognized value, stores the argument and points the global
 *  pointer at D_80012088; other values leave both globals unchanged.
 *  @param a0 Value tested against 3.
 *  @par Globals
 *  D_8009F8C8 receives 3 and D_8009F8C4 receives D_80012088 when matched.
 *  D_80012088 supplies the address stored in D_8009F8C4.
 *  @par Control flow
 *  Branches on whether a0 equals 3; there are no loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May update D_8009F8C8 and D_8009F8C4.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007BFAC(s32 a0)
{
    if (a0 == 3) {
        gDeferredTransition = a0;
        D_8009F8C4 = D_80012088;
    }
}
