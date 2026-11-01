#include "game/types.h"

struct Unk50DACCell {
    s16 unk00;
    s16 x;
    s16 unk04;
    s16 z;
    s32 unk08;
};

struct Unk50DACMap {
    s16 unk00;
    s16 count;
    s32 unk04;
    struct Unk50DACCell cells[1];
};

struct Unk50E0CCellAccess {
    u16 pad0;
    u16 pad2;
    u16 pad4;
    u16 unk6;
    u16 unk8;
    u16 unkA;
};

struct Unk50E0CArg0 {
    u16 unk00;
    s16 unk02;
    u16 unk04;
    u16 unk06;
    s16 unk08;
    s16 unk0A;
    u16 unk0C;
    u16 unk0E;
    u16 unk10;
    u8 unk12;
};

struct Unk50E0CArg2 {
    u8 *unk00;
};

struct Unk50E0CArg3 {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x54 - 0x28];
    s32 unk54;
    u8 pad58[0x78 - 0x58];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Unk50E0COut {
    u8 *unk00;
    u16 unk04;
    u16 unk06;
    u16 unk08;
    u16 unk0A;
    u16 unk0C;
    u16 unk0E;
    u8 pad10[0x1C - 0x10];
    s32 unk1C;
    s16 unk20;
    u8 pad22[0x26 - 0x22];
    u16 unk26;
    u8 pad28[0x2E - 0x28];
    u16 unk2E;
    s16 unk30;
    s16 unk32;
    u16 unk34;
    s16 unk36;
    s16 unk38;
    u16 unk3A;
    u32 unk3C;
    u16 unk40;
    u16 unk42;
    s16 unk44;
    s16 unk46;
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50;
    s16 unk52;
    s16 unk54;
    s16 unk56;
    u16 unk58;
    u16 unk5A;
    s32 unk5C;
    u8 pad60[0x64 - 0x60];
    s32 unk64;
    s32 unk68;
    s32 unk6C;
};

extern s32 func_80050DAC(struct Unk50DACMap *map, s32 *position);
extern s32 func_8004F330(struct Unk50E0COut *out, struct Unk50E0CArg3 *arg3, s32 index);
extern void func_8001AB78(s32 scale, s32 *vector);
extern s32 func_8001A100(s32 *vector);
extern s32 func_80095C30(s32 x, s32 y);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_80050E0C(struct Unk50E0CArg0 *arg0, struct Unk50E0COut *out, struct Unk50E0CArg2 *arg2, struct Unk50E0CArg3 *arg3, s32 *arg4)
{
    struct Unk50DACMap *map;
    struct Unk50E0CCellAccess *cell;
    s16 index;
    s32 length;
    s32 quotient;
    s32 angle;
    s32 angle2;
    s32 a0;
    s32 a1;
    s32 a2;
    s32 a3;
    s32 t0;
    s32 v0;
    s32 v1;

    if (arg2 == 0) {
        out->unk00 = 0;
        return;
    }

    arg3->unk24 |= 0x80000000U;
    map = (struct Unk50DACMap *)(arg2->unk00 + 0xC);
    out->unk00 = (u8 *)map;
    index = func_80050DAC(map, arg4);

    cell = (struct Unk50E0CCellAccess *)((u8 *)map + 4) + index;
    out->unk48 = out->unk4A = index;
    out->unk04 = cell->unk6;
    out->unk06 = cell->unk8;
    out->unk08 = cell->unkA;

    out->unk1C = 0;
    out->unk5C = 0;
    out->unk42 = arg0->unk10;
    out->unk3C = arg0->unk00;
    out->unk3C |= (u32)arg0->unk12 << 24;

    out->unk44 = func_8004F330(out, arg3, out->unk48);
    out->unk46 = -1;
    if (out->unk44 < 0) {
        out->unk3C |= 0x40;
    }

    index = out->unk44;
    cell = (struct Unk50E0CCellAccess *)((u8 *)map + 4) + index;
    out->unk0A = cell->unk6;
    out->unk0C = cell->unk8;
    out->unk0E = cell->unkA;
    out->unk0A -= out->unk04;
    out->unk0C -= out->unk06;
    out->unk0E -= out->unk08;

    arg3->unk78 = (s16)out->unk0A << 8;
    arg3->unk7C = (s16)out->unk0C << 8;
    arg3->unk80 = (s16)out->unk0E << 8;

    length = func_8001A100(&arg3->unk78);
    out->unk20 = length >> 8;

    if ((((length >> 31) + length) ^ (length >> 31)) >= 2) {
        quotient = 0x80000000U / (length >> 1);
    } else {
        quotient = 0;
    }
    func_8001AB78(quotient, &arg3->unk78);

    angle = func_80095C30(arg3->unk78, arg3->unk80) & 0xFFF;
    out->unk4C = out->unk4E = out->unk50 = angle;
    func_8001846C(out->unk50, &out->unk64, &out->unk6C);

    a3 = arg3->unk78;
    if (a3 < 0) {
        a3 = -a3;
    }
    a2 = arg3->unk80;
    if (a2 < 0) {
        a2 = -a2;
    }
    if (a3 < a2) {
        a3 ^= a2;
        a2 ^= a3;
        a3 ^= a2;
    }
    if ((a3 >> 1) < a2) {
        a0 = a3 + a2;
        t0 = a0 << 6;
        v1 = a0 << 3;
        t0 += v1;
        v1 = a0 << 1;
        t0 += v1;
        t0 += a3;
        v1 = a2 << 5;
        a0 = t0 + v1;
        a2 = a3 - a2;
        a3 = a0 >> 7;
    }
    a1 = a3 + (a2 >> 2) - (a2 >> 6);
    angle2 = func_80095C30(arg3->unk7C, a1);
    angle2 = (angle2 << 12) >> 12;
    v1 = (0xFFF - angle2) & 0xFFF;
    out->unk52 = out->unk54 = out->unk56 = v1;
    out->unk68 = arg3->unk7C;

    v1 = arg0->unk04;
    out->unk26 = v1;
    a1 = (s16)v1 << 4;
    if (a1 > 0 && (out->unk3C & 0x10)) {
        a1 = 0;
    }

    v0 = (s16)out->unk06;
    arg3->unk84 = 0;
    arg3->unk54 = a1 + (v0 << 8);

    v0 = ((arg0->unk02 << 8) / 300) >> 4;
    out->unk32 = v0;
    out->unk30 = v0;

    out->unk36 = (arg0->unk08 << 8) / 300;

    out->unk38 = (arg0->unk0A << 8) / 300;

    out->unk34 = arg0->unk06;
    out->unk3A = arg0->unk0C;
    out->unk2E = arg0->unk0E;
    out->unk40 = 0;
    out->unk5A = 0;
    out->unk58 = 0;
}
