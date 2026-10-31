#include "game/types.h"

struct Sub80021AD0 {
    u8 unk00[0xA];
    u16 unk0A;
    u16 unk0C;
    u8 unk0E[2];
    u32 unk10;
};

struct Arg80021AD0 {
    struct Sub80021AD0 *unk00;
    u8 unk04[8];
    s32 unk0C;
};

struct Local80021AD0 {
    u32 unk00;
    u32 unk04;
    s16 unk08;
    s16 unk0A;
    u8 unk0C[8];
    s16 unk14;
    u8 unk16[6];
    u32 unk1C;
    u32 unk20;
    u8 unk24[0x16];
    u16 unk3A;
    u16 unk3C;
};

extern void func_80099C58(s32, void *);

void func_80021AD0(struct Arg80021AD0 *arg0, s16 arg1, s16 arg2, s16 arg3)
{
    struct Sub80021AD0 *v1;
    struct Local80021AD0 sp;

    v1 = arg0->unk00;
    sp.unk00 = 1 << arg0->unk0C;
    sp.unk04 = 0x70093;
    sp.unk08 = arg2;
    sp.unk0A = arg3;
    sp.unk14 = arg1;
    sp.unk1C = v1->unk10;
    sp.unk3A = v1->unk0A;
    sp.unk3C = v1->unk0C;
    sp.unk20 = v1->unk10;
    func_80099C58(arg0->unk0C, &sp);
}
