// Categories: Game Engine > State
#include "game/types.h"

extern s8 *D_8009F468;
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");

/**
 *  @brief Tests the current indexed state and returns a status code.
 *
 *  Checks an enable byte and selected bytes in the current state object;
 *  the conditions differ when its byte at offset five equals two.
 *  @return 2 when the applicable conditions match, otherwise 0.
 *  @par Globals
 *  D_8009F468 points to the state bytes; D_8009F8B8 selects an index.
 *  D_8009F8C0 supplies the indexed enable byte.
 *  @par Control flow
 *  For type two, checks index zero or one against their respective values.
 *  Otherwise, checks whether the indexed state byte equals one.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8007B0D4(void)
{
    s8 *p;
    register s32 t __asm__("$5");
    register s32 r __asm__("$6");
    s32 idx;

    p = D_8009F468;
    t = p[5];
    r = 0;
    if (t == 2) {
        idx = gActivePlayerIndex;
        if (gPlayerMenuDepth[idx] != 0) {
            if (idx == 0 && p[0] == t) {
                r = 2;
            } else if (idx == 1 && p[1] == 5) {
                r = 2;
            }
        }
    } else {
        idx = gActivePlayerIndex;
        if (gPlayerMenuDepth[idx] != 0 && p[idx] == 1) {
            r = 2;
        }
    }
    return r;
}
