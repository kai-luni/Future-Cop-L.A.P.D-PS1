// Categories: Input > Profile State
#include "game/types.h"

extern u32 gPadWords[2] __asm__("D_8009F7C0");
extern u32 gMapPadButtons(u32 profile) __asm__("func_8001E620");
extern s32 gHandlePlayerEntityMsg(s32 arg0) __asm__("func_8005908C");
extern s32 gEntityBaseHandler() __asm__("func_8002AD58");

/**
 *  @brief Refreshes two stored profile values.
 *
 *  Each entry in the global array is replaced with the result of querying
 *  the corresponding profile index.
 *  @par Globals
 *  D_8009F7C0 receives the two returned values.
 *  @par Control flow
 *  A two-iteration loop processes profile indices zero and one.
 *  @par Calls
 *  func_8001E620 supplies the value for each profile index.
 *  @par Side effects
 *  Overwrites both entries of the global array.
 *  @par Callers
 *  Confirmed: func_8002703C, func_80027E68.
 */
void func_80057864(void)
{
    register u32 *p __asm__("$17");
    s32 i;

    for (i = 0, p = gPadWords; i < 2; i++) {
        *p = gMapPadButtons(i);
        p++;
    }
}
