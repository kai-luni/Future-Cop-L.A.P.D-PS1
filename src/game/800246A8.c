#include "game/types.h"

extern s32 D_8009F280;

extern void func_8002A31C(s32 arg0, s32 arg1);
extern s32 func_8002A398(void);
extern void func_80022B28(s32 start, s32 length, s32 channel, u8 looping);

s32 func_800246A8(s32 arg0)
{
    s32 *v1;
    u8 *s0;

    s0 = 0;
    if (D_8009F280 != 0) {
        func_8002A31C(D_8009F280, 1);
        D_8009F280 = 0;
    }
    v1 = (s32 *)func_8002A398();
    D_8009F280 = (s32)v1;
    if (v1 != 0) {
        (void)*(volatile s32 *)(v1 + 1);
        s0 = (u8 *)v1 + 0x1C;
    }
    if (s0 != 0) {
        func_80022B28((s32)(s0 + 0x2FC0), 0x2FC0, 1, 0);
        func_80022B28((s32)s0, 0x2FC0, 0, 0);
    }
    return D_8009F280;
}
