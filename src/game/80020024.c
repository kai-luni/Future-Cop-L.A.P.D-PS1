#include "game/types.h"

struct Ctx {
    s32 unk000;
    u8 unk004[0x8C8];
    void (*unk8CC)(void);
    u8 unk8D0[0x24];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    u8 unk908[0x9C];
    s32 unk9A4;
    u8 unk9A8[0x1208];
    u32 unk1BB0;
};

extern volatile struct Ctx *D_8009AB08;

extern void func_8008D640(s32 value);

void func_80020024(void)
{
    if (D_8009AB08->unk1BB0++ >= 0x3D) {
        D_8009AB08->unk1BB0 = 0;
        D_8009AB08->unk8F8 = 0;
        D_8009AB08->unk8F4 = 0;
        D_8009AB08->unk904 = 0;
        D_8009AB08->unk900 = -3;
        D_8009AB08->unk9A4 = 1;
        func_8008D640(D_8009AB08->unk8FC);
    }
    D_8009AB08->unk8CC();
}
