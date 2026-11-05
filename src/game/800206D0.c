#include "game/types.h"

struct Ctx {
    s32 unk000;
    u8 unk004[0x8F0];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    s32 unk908;
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    s32 unk9A0;
    s32 unk9A4;
    s32 unk9A8;
    s32 unk9AC;
    u8 unk9B0[0x1200];
    s32 unk1BB0;
};

extern volatile struct Ctx *D_8009AB08;

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_80020530(void);
extern void func_80020120(void);
extern void func_8008D640(s32 value);

void func_800206D0(void)
{
    s32 t = D_8009AB08->unk000;

    D_8009AB08->unk8F4 = 0;
    D_8009AB08->unk8F8 = 0;
    D_8009AB08->unk910 = 0;
    D_8009AB08->unk8FC = t;
    D_8009AB08->unk900 = -2;
    D_8009AB08->unk904 = 0;
    D_8009AB08->unk908 = 1;
    D_8009AB08->unk90C = 0;
    D_8009AB08->unk994 = 0;
    D_8009AB08->unk998 = 0;
    D_8009AB08->unk9A0 = 3;
    D_8009AB08->unk9AC = 3;
    D_8009AB08->unk9A8 = 1;
    D_8009AB08->unk9A4 = 0;
    D_8009AB08->unk1BB0 = 0;

    func_8001E4C0(D_8009AB08->unk914, 0, 0x80);
    func_8001E4C0(D_8009AB08->unk9B0, 0, 0x1200);
    func_80020530();
    func_80020120();
    func_8008D640(D_8009AB08->unk8FC);
}
