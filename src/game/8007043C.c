#include "game/types.h"

typedef struct {
    u8 unk00[0x34];
    u16 unk34;
    u8 unk36;
    u8 pad37;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    u8 unk3E;
    u8 unk3F;
    u8 unk40;
    u8 unk41;
    u8 unk42;
    u8 unk43;
    u8 unk44;
    u8 unk45;
    u8 unk46;
    u8 unk47;
    u8 unk48;
    u8 unk49;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    u16 unk50;
    u8 unk52;
    u8 pad53;
    u16 unk54;
    u16 unk56;
} Source7043C;

typedef struct {
    u8 unk00[8];
    s16 unk08;
    u16 unk0A;
    u8 unk0C[0x12 - 0x0C];
    u16 unk12;
    u8 unk14[0x24 - 0x14];
    s32 unk24;
    u8 unk28[0x66 - 0x28];
    u16 unk66;
    u16 unk68;
    u16 unk6A;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    u8 unk70;
    u8 unk71;
    u8 unk72;
    u8 unk73;
    u8 unk74;
    u8 unk75;
    u8 unk76;
    u8 unk77;
    u8 unk78[0x8C - 0x78];
    u16 unk8C;
    u8 unk8E;
    u8 unk8F;
    u16 unk90;
    u8 unk92[0x94 - 0x92];
    s16 unk94;
    s16 unk96;
    s16 unk98;
    u16 unk9A;
} Dest7043C;

typedef struct {
    u8 unk00[0x18];
    s32 x;
    s32 y;
    s32 z;
} Work7043C;

extern u8 D_8006FF44[];
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C14C(Dest7043C *dest, Source7043C *src);
extern s32 func_80019668(void);
extern void func_800188D0(s32 x, s32 y, s32 z, Work7043C *work);

void *func_8007043C(Source7043C *src)
{
    Dest7043C *dest;
    Work7043C work;
    s32 x;
    s32 y;
    s16 sval;
    u16 value;
    u16 src50;
    u8 dest8E;

    dest = func_80017EBC(0x9C, 1);
    if (dest == 0) {
        return 0;
    }

    func_8003C14C(dest, src);

    dest->unk24 = (s32)D_8006FF44;
    dest->unk12 = src->unk34;
    dest->unk66 = src->unk38;
    dest->unk68 = src->unk3A;
    dest->unk6A = src->unk3C;
    dest->unk6C = src->unk3E;
    dest->unk6D = src->unk3F;
    dest->unk6E = src->unk40;
    dest->unk6F = src->unk41;
    dest->unk70 = src->unk42;
    dest->unk71 = src->unk43;
    dest->unk72 = src->unk44;
    dest->unk73 = src->unk45;
    dest->unk74 = src->unk46;
    dest->unk75 = src->unk47;
    dest->unk76 = src->unk48;
    dest->unk77 = src->unk49;
    dest->unk8C = src->unk56;
    dest->unk8E = src->unk36;

    if (src->unk4A < 0) {
        x = (s32)((u32)func_80019668() % (u32)-src->unk4A);
    } else {
        x = src->unk4A;
    }

    if (src->unk4C < 0) {
        y = (s32)((u32)func_80019668() % (u32)-src->unk4C);
    } else {
        y = src->unk4C;
    }

    func_800188D0(x, -y, 0, &work);

    dest->unk94 = (s16)(((work.x >> 4) * src->unk4E) >> 12);
    dest->unk96 = (s16)(((work.y >> 4) * src->unk4E) >> 12);
    dest->unk98 = (s16)(((work.z >> 4) * src->unk4E) >> 12);

    dest->unk8F = src->unk52;

    sval = (s16)src->unk54;
    value = *(volatile u16 *)&src->unk54;
    if (sval < 0) {
        value = (u16)((u32)func_80019668() % (u32)(-(s32)(s16)src->unk54));
    }
    dest->unk90 = value;

    src50 = src->unk50;
    dest8E = dest->unk8E;
    dest->unk0A = 0;
    dest->unk9A = src50;
    dest->unk08 = dest8E + 1;

    return dest;
}
