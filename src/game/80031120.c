#include "game/types.h"

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x08];
    u16 unk14;
    u8 unk16[0x0E];
    u32 flags;
    u8 unk28[0x50];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 unk88[0x0C];
    s32 unk94;
    u8 unk98[0x08];
    u8 flagsA0;
    u8 unkA1[0x03];
    s32 unkA4;
    u8 unkA8[0x1C];
    u8 *unkC4;
    u8 *unkC8;
    s32 unkCC;
    u8 unkD0[0x02];
    s16 unkD2;
    s16 unkD4;
    u8 unkD6[0x02];
    u16 unkD8;
    u8 unkDA[0x06];
    s32 unkE0;
};

extern s32 D_8009FAA4;
extern void func_8004D958(struct Entity *entity);
extern s32 func_80069D68(s32 script, s32 arg1, s32 arg2, struct Entity *entity);

s32 func_80031120(struct Entity *arg0, struct Entity *arg1)
{
    u8 *oldC8;
    u8 flags;

    flags = arg0->flagsA0;
    arg0->unkA4 = D_8009FAA4;

    if (flags & 8) {
        oldC8 = arg1->unkC8;
        arg1->unkC4 = oldC8;
        arg1->unkD8 = *(u16 *)(oldC8 + 8);
        arg1->unkC8 = (u8 *)arg0;
        arg1->unkCC = arg0->unk08;
        arg1->unkE0 = 1;
        arg1->unkD4 = 0xDA;
        arg1->unkD2 = 0x100;
        arg1->unk78 >>= 1;
        arg1->unk7C >>= 1;
        arg1->unk80 >>= 1;
        arg1->unk84 = 0;
    }

    if (arg0->flagsA0 & 1) {
        arg1->flags |= 0x4000000;
        func_8004D958(arg1);
    }

    if (arg0->flagsA0 & 2) {
        if (arg0->unk94 != 0) {
            func_80069D68(arg0->unk94, 0, arg1->unk14 & 0x7FFF, arg0);
            return 0;
        }
    } else {
        func_8004D958(arg0);
    }

    return 0;
}
