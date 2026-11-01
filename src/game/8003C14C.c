#include "game/types.h"

struct UnkC14C {
    s32 unk00;
    s16 unk04;
    u8 unk06;
    u8 unk07;
    u8 unk08[6];
    u8 unk0E;
    u8 unk0F;
    u8 unk10;
    u8 unk11;
    u8 unk12[2];
    u16 unk14;
    u16 unk16;
    u16 unk18;
    u16 unk1A;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
};

struct UnkC14CSrc {
    u8 unk00[0x15];
    u8 unk15;
    u8 unk16;
    u8 unk17[7];
    u8 unk1E;
    u8 unk1F;
    u32 unk20;
    u32 unk24;
    u8 unk28;
    u8 unk29;
    s16 unk2A;
    u16 unk2C;
    u8 unk2E[4];
    u16 unk32;
};

extern s32 D_8009AA38[9];

void func_8003C14C(struct UnkC14C *dest, struct UnkC14CSrc *src)
{
    dest->unk06 = src->unk15;
    dest->unk11 = src->unk16;

    dest->unk34 = D_8009AA38[0];
    dest->unk38 = D_8009AA38[1];
    dest->unk3C = D_8009AA38[2];
    dest->unk40 = D_8009AA38[3];
    dest->unk44 = D_8009AA38[4];
    dest->unk48 = D_8009AA38[5];
    dest->unk4C = D_8009AA38[6];
    dest->unk50 = D_8009AA38[7];
    dest->unk54 = D_8009AA38[8];

    dest->unk28 = 0;
    dest->unk2C = 0;
    dest->unk30 = 0;
    dest->unk58 = 0;
    dest->unk5C = 0;
    dest->unk60 = 0;

    dest->unk16 = (src->unk20 * 75) >> 14;
    dest->unk04 = (src->unk24 * 75) >> 14;
    dest->unk07 = src->unk28;
    dest->unk24 = 0;
    dest->unk0F = 0;
    dest->unk1A = src->unk32;
    dest->unk10 = 0;
    dest->unk18 = src->unk2C;
    dest->unk0E = src->unk1E;
    dest->unk00 = 0;
    dest->unk1C = 0;

    if (dest->unk06 != 0 && dest->unk04 < 0) {
        dest->unk04 = 0;
    }

    dest->unk14 = ((u32)src->unk2A * 75) >> 10;
}
