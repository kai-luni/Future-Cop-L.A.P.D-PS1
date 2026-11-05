#include "game/types.h"

extern s8 *D_8009F468;
extern s32 D_8009F8B8;
extern s8 D_8009F8C0[];

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
        idx = D_8009F8B8;
        if (D_8009F8C0[idx] != 0) {
            if (idx == 0 && p[0] == t) {
                r = 2;
            } else if (idx == 1 && p[1] == 5) {
                r = 2;
            }
        }
    } else {
        idx = D_8009F8B8;
        if (D_8009F8C0[idx] != 0 && p[idx] == 1) {
            r = 2;
        }
    }
    return r;
}
