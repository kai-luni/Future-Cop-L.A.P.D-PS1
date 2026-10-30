#include "game/types.h"

struct Unk3D680;
struct Unk3D680Source;
struct Unk3D680Parent;
struct Unk3D680Entry;

typedef u8 (*Unk3D680SourceFunc)(s32, struct Unk3D680Source *, s32, s32);

struct Unk3D680Source {
    u8 unk00[4];
    Unk3D680SourceFunc unk04;
    s32 unk08;
    u8 unk0C[0x1C];
    u8 unk28[0x6A];
    u8 unk92;
    u8 unk93;
};

struct Unk3D680Entry {
    u8 unk00[0x32];
    u16 unk32;
};

struct Unk3D680Parent {
    s32 unk00;
    u8 unk04[0x0C];
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 unk40[4];
    u8 *unk44;
    u8 unk48;
    u8 unk49;
};

struct Unk3D680 {
    struct Unk3D680 *next;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    u8 *unk34;
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    u8 unk40;
    u8 unk41;
    u8 unk42;
    u8 unk43;
    s32 unk44;
    s32 unk48;
};

extern s32 D_8009AA38[9];
extern struct Unk3D680 *D_8009F320;
extern void *D_800A7040[][0x100];

extern void *func_80017EBC(s32 size, s32 tag);
extern u32 *func_8003B3D8(u32 column, u32 row);

struct Unk3D680 *func_8003D680(struct Unk3D680Source *source, u8 type, s32 row,
                               struct Unk3D680Parent *parent, s32 arg4, u8 arg5)
{
    struct Unk3D680 *object;
    struct Unk3D680Entry *entry;
    u32 *result;
    u8 column;
    u8 value;
    s32 parentX;
    s32 parentY;
    s32 parentZ;

    object = func_80017EBC(0x4C, 1);
    if (object == 0) {
        return 0;
    }

    object->unk34 = 0;
    object->unk44 = 0;
    object->unk48 = arg4;

    if (source != 0) {
        object->unk04 = (s32)source;
        object->unk08 = source->unk08;
        if (source->unk08 == 0) {
            object->unk08 = -1;
        }
        object->unk38 = 0;
        object->unk34 = (u8 *)(object->unk04 + 0x28);
        object->unk40 = source->unk92;
        object->unk41 = source->unk93;
        object->unk42 = source->unk04(0x10, source, 0, 0);
    } else {
        object->unk42 = 1;
        object->unk41 = 0;
        object->unk40 = 0;

        if (parent != 0 && (parent->unk49 & 8)) {
            object->unk04 = parent->unk1C;
            object->unk08 = parent->unk20;
            object->unk0C = parent->unk24;
            object->unk10 = parent->unk28;
            object->unk14 = parent->unk2C;
            object->unk18 = parent->unk30;
            object->unk1C = parent->unk34;
            object->unk20 = parent->unk38;
            object->unk24 = parent->unk3C;
        } else {
            object->unk04 = D_8009AA38[0];
            object->unk08 = D_8009AA38[1];
            object->unk0C = D_8009AA38[2];
            object->unk10 = D_8009AA38[3];
            object->unk14 = D_8009AA38[4];
            object->unk18 = D_8009AA38[5];
            object->unk1C = D_8009AA38[6];
            object->unk20 = D_8009AA38[7];
            object->unk24 = D_8009AA38[8];
        }
        object->unk38 = 1;
    }

    if (parent != 0) {
        if (parent->unk49 & 0x40) {
            object->unk34 = parent->unk44;
        }
        if (object->unk38 != 0 && (parent->unk49 & 4)) {
            parentX = parent->unk10;
            parentY = parent->unk14;
            parentZ = parent->unk18;
            object->unk28 = parentX;
            object->unk2C = parentY;
            object->unk30 = parentZ;
        }
    }

    if (object->unk34 == 0) {
        if (parent != 0 && (parent->unk49 & 1) && parent->unk00 > 0) {
            object->unk3C = parent->unk00 >> 4;
        } else {
            object->unk3C = 0x100;
        }
    } else {
        object->unk3C = *(s32 *)(object->unk34 + 0x40) >> 4;
    }

    column = type & 0xFF;
    result = func_8003B3D8(column, row);
    entry = (struct Unk3D680Entry *)D_800A7040[row][column];

    if (entry != 0 && result != 0 &&
        ((s32)arg5 != 0 || (entry->unk32 & 0x40) || (*(u8 *)result & 4))) {
        value = 4;
    } else {
        value = 0;
    }
    object->unk39 = value;

    object->unk3A = 0;
    object->unk3E = 0;
    object->next = D_8009F320;
    D_8009F320 = object;
    return object;
}
