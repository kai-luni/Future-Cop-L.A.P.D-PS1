#include "game/types.h"

struct Unk800A0B70 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 unk0C[0x20];
    s32 unk2C;
    u8 unk30[0x14];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

extern struct Unk800A0B70 D_800A0B70;
extern volatile u8 D_8009F6A8;

extern void func_80029DD8(void);
extern void func_80029520(void);
extern void func_80019584(void);
extern void func_8001D3A8(s32 arg0);

void func_80029EB8(void)
{
    s32 old;
    register s32 pending __asm__("$4");

    old = D_800A0B70.unk00;
    D_800A0B70.unk00 = -1;
    func_80029DD8();
    func_80029520();
    D_800A0B70.unk04 = 0;
    D_800A0B70.unk08 = 0;
    D_800A0B70.unk44 = 0;
    D_800A0B70.unk48 = 0;
    D_800A0B70.unk4C = 0;
    pending = D_800A0B70.unk2C;
    if (pending != 0) {
        func_80019584();
        D_800A0B70.unk2C = 0;
    }
    if (old != -1) {
        while (D_8009F6A8 != 0) {
        }
        func_8001D3A8(old);
    }
}
