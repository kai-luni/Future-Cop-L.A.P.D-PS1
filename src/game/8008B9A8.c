#include "game/types.h"

extern u32 D_8009C730;
extern s32 func_8008AB80(void *s, u32 a1);

s32 func_8008B9A8(void *s)
{
    u32 flag;
    u32 r0;
    s32 r;
    s32 ret;

    flag = 0;
    if ((*(u8 *)(*(u32 *)((u8 *)s + 0x3C)) >> 4) == 8) {
        flag = (*(u8 *)((u8 *)s + 0x36) == 0);
    }
    r0 = ((u32 (*)(void *, u32))D_8009C730)(s, flag);
    r = func_8008AB80(s, r0 & 0xFF);
    ret = r;
    if (r == 0x5A) {
        goto end;
    }
    if (r == 0) {
        goto end;
    }
    if (r >= 0) {
        ret = -9;
        goto end;
    }
end:
    return ret;
}
