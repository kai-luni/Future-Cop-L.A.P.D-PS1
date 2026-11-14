#include "game/types.h"

typedef struct {
    u8 pad0[0xC];
    void *unkC;
    u8 pad10[0x26];
    u8 unk36;
} Unk8008B8E8;

extern u32 D_8009C7A8;
extern u32 D_8009C740;
extern u32 D_8009C774;
extern u32 D_8009C7A0;
extern s32 func_8008AB80(Unk8008B8E8 *, void *);

s32 func_8008B8E8(Unk8008B8E8 *arg0)
{
    s32 ret;
    void *a1;

    if (D_8009C7A8 != 0) {
        ((void (*)(void *))D_8009C740)((u8 *)arg0->unkC + 0x1E0);
        ((void (*)(void *))D_8009C740)((u8 *)arg0->unkC + 0x2D0);
    }
    a1 = 0;
    if (arg0->unk36 == 0) {
        a1 = (void *)D_8009C774;
    }
    ret = func_8008AB80(arg0, a1);
    if (ret < 0) {
        return ret;
    }
    if ((ret & 0xF0) == 0) {
        return -9;
    }
    D_8009C7A0 = (ret & 0xF) << 1;
    if (D_8009C7A0 == 0) {
        D_8009C7A0 = 0x20;
    }
    return 0;
}