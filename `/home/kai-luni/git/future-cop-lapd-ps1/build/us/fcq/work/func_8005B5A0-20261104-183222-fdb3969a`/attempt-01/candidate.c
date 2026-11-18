#include "game/types.h"

struct B5A0_B {
    u8 unk00[4];
    void (*unk04)(s32, struct B5A0_B *, void *, s32);
    u8 unk08[0x54];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[6];
    s16 unk6E;
};

struct B5A0_C {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s16 unk10;
    u8 unk12;
};

struct B5A0_A {
    u8 unk00[8];
    u8 *unk08;
    u8 unk0C[0x10];
    u16 unk1C;
    u8 unk1E[0xC];
    u8 flags2A;
};

struct B5A0_packet {
    s32 unk00;
    u16 unk04;
    u8 unk06;
    u8 pad;
    u8 *unk08;
};

extern s32 func_80019FCC(s32 *vec);

void func_8005B5A0(struct B5A0_A *a0, struct B5A0_B *a1, struct B5A0_C *a2)
{
    s32 vec[3];
    struct B5A0_packet packet;

    if (a1 == 0) {
        return;
    }
    vec[0] = a2->unk04 - a1->unk5C;
    vec[1] = a2->unk08 - a1->unk60;
    vec[2] = a2->unk0C - a1->unk64;
    if (func_80019FCC(vec) >= ((a2->unk10 + a1->unk6E) * (a2->unk10 + a1->unk6E)) >> 8) {
        return;
    }
    packet.unk06 = 2;
    packet.unk04 = a0->unk1C;
    packet.unk00 = *(u16 *)(a0->unk08 + 0x14);
    packet.unk08 = a0->unk08;
    a1->unk04(0xE, a1, &packet, 0);
    a2->unk12 = 0xA;
    if (a0->flags2A & 4) {
        a0->flags2A |= 8;
    }
}
