#include "game/types.h"

extern s32 D_8009FA00;
extern s32 D_8009F658;
extern s32 D_8009F9C4;
extern u8 *D_8009FA94;
extern u8 D_8009F2E4[];

extern void func_800247A8(void);
extern s32 func_800833A8(s32 arg0);
extern void func_8008CD30(void *dst, void *src, s32 len, s32 arg3);
extern void func_80024904(s32 arg);
extern void func_80024A24(s32 arg);

s32 func_80024E3C(s32 arg0)
{
    u8 buf[0x28];
    s32 r;

    func_800247A8();
    D_8009FA00 = 1;
    D_8009F658 = 0;
    *(s32 *)(D_8009FA94 + 0x10) = 4;

    r = func_800833A8(arg0);
    func_8008CD30(buf, D_8009F2E4, r, arg0);

    if (D_8009F9C4 != 0) {
        func_80024904((s32)buf);
    } else {
        func_80024A24((s32)buf);
    }

    return 1;
}
