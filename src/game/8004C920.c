#include "game/types.h"

struct Unk4C920Source {
    u8 unk00[0x34];
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 pad37;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    s16 unk40;
    s16 unk42;
    u16 unk44;
    u16 unk46;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
};

struct Unk4C920Work {
    u8 unk00[0x18];
    s32 x;
    s32 y;
    s32 z;
};

struct Unk4C920 {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    u8 unk0C[0x24 - 0x0C];
    s32 unk24;
    u8 unk28[0x64 - 0x28];
    u8 unk64;
    u8 unk65;
    u16 unk66;
    u16 unk68;
    u16 unk6A;
    u16 unk6C;
    u16 unk6E;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    u16 unk76;
    u16 unk78;
    u8 unk7A;
    u8 unk7B;
    u8 unk7C;
    s8 unk7D;
    u8 unk7E;
    u8 unk7F;
    u8 unk80[0x88 - 0x80];
};

extern u8 D_8004C324[];
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8003C14C(struct Unk4C920 *dest, struct Unk4C920Source *src);
extern void func_800188D0(s32, s32, s32, struct Unk4C920Work *);

void *func_8004C920(struct Unk4C920Source *src)
{
    struct Unk4C920 *dest;
    struct Unk4C920Work work;

    dest = func_80017EBC(0x88, 1);
    if (dest == 0) {
        return 0;
    }
    func_8003C14C(dest, src);
    dest->unk66 = src->unk38;
    dest->unk68 = src->unk3A;
    dest->unk6A = src->unk3C;
    dest->unk6C = src->unk3E;
    func_800188D0(src->unk40, -src->unk42, 0, &work);
    dest->unk70 = work.x >> 4;
    dest->unk72 = work.y >> 4;
    dest->unk74 = work.z >> 4;
    dest->unk76 = src->unk44;
    dest->unk78 = src->unk46;
    dest->unk7C = src->unk48;
    dest->unk24 = (s32)D_8004C324;
    dest->unk64 = src->unk34;
    dest->unk65 = src->unk35;
    dest->unk7A = src->unk36;
    dest->unk7B = src->unk4A;
    if ((u32)src->unk49 >= 0x33) {
        dest->unk7D = 0x32;
    } else {
        dest->unk7D = src->unk49;
    }
    if (dest->unk7D < 0 || (dest->unk7B & 8)) {
        dest->unk0A = 0x14;
    } else {
        dest->unk0A = dest->unk7D;
    }
    dest->unk08 = 0;
    return dest;
}
