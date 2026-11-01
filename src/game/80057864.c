#include "game/types.h"

extern u32 D_8009F7C0[2];
extern u32 func_8001E620(u32 profile);
extern s32 func_8005908C(s32 arg0);
extern s32 func_8002AD58();

void func_80057864(void)
{
    register u32 *p __asm__("$17");
    s32 i;

    for (i = 0, p = D_8009F7C0; i < 2; i++) {
        *p = func_8001E620(i);
        p++;
    }
}
