#include "game/types.h"

struct Unk8009AB08 {
    u8 unk0000[0x8F8];
    s32 unk8F8;
    u8 unk8FC[0x10];
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    u8 unk9A0[4];
    s32 unk9A4;
    u8 unk9A8[0x120C];
    s32 unk1BB4;
};

extern struct Unk8009AB08 *D_8009AB08;
extern s32 func_8001F180(u32 arg0, u32 arg1, u32 arg2, u32 arg3);

void func_8001F1B8(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    volatile struct Unk8009AB08 *p;

    if (func_8001F180(arg0, arg1, arg2, arg3) == 0) {
        p = D_8009AB08;
        p->unk998 = arg0;
        p->unk99C = arg1;
        p->unk910 = arg2;
        p->unk90C = arg3;
        p->unk994 = 0;
        p->unk9A4 = 0;
        p->unk8F8 = 2;
    } else {
        D_8009AB08->unk9A4 = 1;
    }
}
