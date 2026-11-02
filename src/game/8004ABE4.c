#include "game/types.h"

struct Unk4ABE4Inner {
    u8 pad00[0x34];
    u8 *unk34;
};

struct Unk4ABE4Parent {
    u8 pad00[0x20];
    s32 unk20;
};

struct Unk4ABE4Alloc {
    s32 unk00;
    s16 unk04;
    s16 unk06;
    u16 unk08;
    u16 unk0A;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    u16 unk18;
    u8 pad1A[2];
};

struct Unk4ABE4Arg1 {
    u8 pad00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x30];
    u16 unk40;
    u8 pad42[7];
    u8 unk49;
};

struct Unk4ABE4 {
    u8 pad00[0x0C];
    s16 unk0C;
    u8 unk0E;
    u8 unk0F;
    u8 unk10;
    u8 pad11[5];
    u16 unk16;
    u8 pad18[2];
    u16 unk1A;
    struct Unk4ABE4Inner *unk1C;
    struct Unk4ABE4Parent *unk20;
    u8 pad24[0x41];
    u8 unk65;
    u8 pad66[4];
    s16 unk6A;
    s16 unk6C;
    s16 unk6E;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    u16 unk76;
    u16 unk78;
    u8 pad7A[2];
    s8 unk7C;
    u8 pad7D[3];
    struct Unk4ABE4Alloc *unk80;
    s32 unk84;
};

extern void *func_80017EBC(s32 size, s32 tag);
extern s32 func_80019668(void);
extern void func_80010820(u8 *arg0);
extern void func_8001006C(void *arg0, void *arg1, s32 arg2);
extern void func_8003E984(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

s32 func_8004ABE4(struct Unk4ABE4 *object, struct Unk4ABE4Arg1 *arg1)
{
    u8 *context;
    s32 value;
    s32 delta;
    s32 scale;
    s16 turnScale;

    context = 0;
    if (object->unk1C != 0) {
        context = object->unk1C->unk34;
    }

    if (context == 0) {
        object->unk0F |= 1;
        return 0;
    }

    object->unk80 = func_80017EBC(0x1C, 1);
    if (object->unk80 == 0) {
        return 0;
    }

    object->unk80->unk00 = ((func_80019668() & 0x1F) + 0xA0) << 3;
    object->unk80->unk00 = (object->unk80->unk00 * object->unk7C) / 100;
    object->unk80->unk00 = (-object->unk80->unk00) * 16;

    delta = (s16)object->unk78 - (s16)object->unk76;
    object->unk80->unk04 = 0;
    object->unk80->unk06 = 0;

    if (delta > 0) {
        object->unk80->unk08 = object->unk76 + (((u32)(delta * (func_80019668() & 0xFFF))) >> 12);
        object->unk80->unk0A = object->unk76 + (((u32)(delta * (func_80019668() & 0xFFF))) >> 12);
    } else {
        object->unk80->unk08 = object->unk78;
        object->unk80->unk0A = object->unk78;
    }

    scale = (s16)object->unk6C - (s16)object->unk6A;
    if (scale > 0) {
        scale = object->unk6A + (((u32)(scale * (func_80019668() & 0xFFF))) >> 12);
    } else {
        scale = object->unk6C;
    }

    if (scale != 0) {
        object->unk80->unk0C = (object->unk70 * scale) >> 8;
        object->unk80->unk10 = (object->unk72 * scale) >> 8;
        object->unk80->unk14 = (object->unk74 * scale) >> 8;
    } else {
        object->unk80->unk0C = 0;
        object->unk80->unk10 = 0;
        object->unk80->unk14 = 0;
    }

    if (object->unk1A & 0x104) {
        func_80010820(context + 4);
        func_8001006C((u8 *)object->unk80 + 0x0C, (u8 *)object->unk80 + 0x0C, 1);
    }

    if (arg1 != 0 && (arg1->unk49 & 2)) {
        value = 0;
        if (!(object->unk1A & 0x200)) {
            if (object->unk1A & 0x400) {
                value = -(arg1->unk40 << 3);
            } else {
                value = arg1->unk40 << 3;
            }
        }

        turnScale = (s16)value;
        if (turnScale != 0) {
            object->unk80->unk0C += ((arg1->unk04 >> 4) * turnScale) >> 8;
            object->unk80->unk10 += ((arg1->unk08 >> 4) * turnScale) >> 8;
            object->unk80->unk14 += ((arg1->unk0C >> 4) * turnScale) >> 8;
        }
    }

    if (object->unk20->unk20 != 0) {
        object->unk80->unk18 = object->unk16;
    } else {
        object->unk80->unk18 = 0;
    }

    object->unk0C = 0;
    object->unk16 = 0;
    object->unk84 = 0;

    if (object->unk65 != 0) {
        func_8003E984(object->unk1C, 0, object->unk65, 0, (object->unk10 >> 1) & 1);
    }

    return 1;
}
