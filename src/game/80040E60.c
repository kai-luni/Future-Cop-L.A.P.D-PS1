#include "game/types.h"

struct Unk40E60Source {
    u8 unk00[0x34];
    u8 unk34;
    u8 pad35;
    u16 unk36;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u16 unk3E;
    u8 unk40;
    u8 unk41;
    u8 unk42;
    u8 unk43;
    u8 unk44;
    u8 unk45;
    u8 unk46;
    u8 unk47;
    u16 unk48;
    u16 unk4A;
    s16 unk4C;
    u16 unk4E;
    u16 unk50;
    u16 unk52;
    u16 unk54;
    u16 unk56;
};

struct Unk40E60Work {
    u8 unk00[0x18];
    s32 x;
    s32 y;
    s32 z;
};

struct Unk40E60 {
    u8 unk00[8];
    u16 unk08;
    u16 unk0A;
    u8 unk0C[0x12 - 0x0C];
    u16 unk12;
    u8 unk14[0x10];
    s32 unk24;
    u8 unk28[0x3C];
    u8 unk64;
    u8 pad65;
    u16 unk66;
    u8 unk68[4];
    s32 unk6C;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    u16 unk76;
    s16 unk78;
    u16 unk7A;
    u16 unk7C;
    u16 unk7E;
    u16 unk80;
    u16 unk82;
    u8 unk84[3];
    u8 unk87;
    u8 unk88;
    u8 unk89;
    u8 unk8A;
    u8 unk8B;
    u8 unk8C;
    u8 unk8D;
    u8 unk8E[6];
    u16 unk94;
};

extern s32 D_8009F318;
extern u8 D_80040874[];
extern void *func_80017EBC(s32, s32);
extern void func_8003C14C(struct Unk40E60 *, struct Unk40E60Source *);
extern void func_800188D0(s32, s32, s32, struct Unk40E60Work *);
extern void func_800403D0(void *);

void *func_80040E60(struct Unk40E60Source *src)
{
    struct Unk40E60 *dest;
    struct Unk40E60Work work;

    if (D_8009F318 >= 0x15F) {
        return 0;
    }
    dest = func_80017EBC(0x98, 1);
    if (dest == 0) {
        return 0;
    }
    func_8003C14C(dest, src);
    dest->unk64 = src->unk34;
    dest->unk12 = src->unk36;
    func_800188D0((s32)src->unk38, -(s32)src->unk3A, 0, &work);
    dest->unk70 = ((work.x >> 4) * src->unk3C) >> 12;
    dest->unk72 = ((work.y >> 4) * src->unk3C) >> 12;
    dest->unk74 = ((work.z >> 4) * src->unk3C) >> 12;
    dest->unk82 = src->unk3E;
    dest->unk76 = src->unk4A;
    dest->unk66 = src->unk56;
    dest->unk24 = (s32)D_80040874;
    dest->unk7A = src->unk4E;
    dest->unk7C = src->unk50;
    dest->unk7E = src->unk52;
    dest->unk80 = src->unk54;
    if (src->unk4C == 0) {
        dest->unk78 = -1;
    } else {
        dest->unk78 = 0x12C / src->unk4C;
    }
    dest->unk6C = 0;
    dest->unk87 = src->unk41;
    dest->unk88 = src->unk42;
    dest->unk89 = src->unk43;
    dest->unk8B = src->unk45;
    dest->unk8C = src->unk46;
    dest->unk8D = src->unk47;
    dest->unk94 = src->unk48;
    /* The oracle reads the flag as one word at src+0x3C, while the
     * multipliers above read the same address as a halfword. */
    dest->unk68[0] = *(s32 *)((u8 *)src + 0x3C) != 0;
    dest->unk08 = 0;
    dest->unk0A = 1;
    D_8009F318++;
    func_800403D0(dest);
    return dest;
}
