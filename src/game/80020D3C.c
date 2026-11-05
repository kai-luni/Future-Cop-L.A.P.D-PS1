#include "game/types.h"

extern u32 *D_8009AB08;
extern u8 D_80010B0C[];

extern s32 func_80020C90(void *arg0);
extern void func_8008CD30(void *buf, void *fmt, u32 n, void *arg);
extern void func_8001F1B8(s32 a, s32 b, s32 c, s32 d);
extern s32 func_8001F37C(void);

s32 func_80020D3C(void *arg0, s32 arg1, u32 arg2)
{
    u8 buf[0x100];
    s32 ret;
    s32 r;
    s32 n;

    ret = 0;
    if (arg2 != 0) {
        r = func_80020C90(arg0);
        if (r != 0) {
            func_8008CD30(buf, D_80010B0C, *D_8009AB08 >> 4, arg0);
            n = (arg2 + 0x7F) >> 7;
            func_8001F1B8(r << 6, n, arg1, ret);
            if (func_8001F37C() == 0) {
                ret = n;
            }
        }
    }
    return ret;
}
