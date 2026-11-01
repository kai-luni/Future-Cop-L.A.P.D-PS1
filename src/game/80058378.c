#include "game/types.h"

struct Unk80058378DC {
    u8 unk00[0x24];
    u32 unk24;
};

struct Unk80058378AC {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Unk80058378 {
    u8 unk00[0x1C];
    s16 unk1C;
    u8 unk1E[0x6];
    u32 unk24;
    u8 unk28[0x64];
    s32 unk8C;
    u8 unk90[0x4];
    void *unk94;
    u8 unk98[0x14];
    struct Unk80058378AC *unkAC;
    u8 unkB0[0x2C];
    struct Unk80058378DC *unkDC;
};

struct Unk8009FA94 {
    u8 unk00[0x5C];
    s32 unk5C;
};

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[8];
    u16 unk14;
};

extern s32 D_8009FA0C;
extern struct Unk8009FA94 *D_8009FA94;
extern void *D_8009FA58[2];

extern void func_80058210(void *object, s32 amount);
extern struct Entity *func_8004D664(s32 id);
extern void func_80069D68(void *arg0, s32 arg1, s32 arg2, void *arg3);

void func_80058378(struct Unk80058378 *object, s32 amount, s32 source)
{
    struct Entity *entity;
    s32 i;
    register void **p __asm__("$17");

    func_80058210(object, amount);

    if (object->unk1C < 0) {
        object->unk8C = source;
    }

    if (amount > 0) {
        object->unk24 |= 0x02000000;
        object->unkDC->unk24 |= 0x02000000;
    }

    if (object->unk94 != 0) {
        entity = func_8004D664(source & 0x3FFFFFFF);
        func_80069D68(object->unk94, (u32)object->unk1C >> 31,
                      entity != 0 ? entity->unk14 & 0x3FFF : -1, object);
    }

    if (D_8009FA0C >= 2 && D_8009FA94->unk5C == 0) {
        i = 0;
        p = D_8009FA58;
        do {
            if (object->unkAC->unkC1 != i && *p != 0) {
                func_80058210(*p, amount);
            }
            i++;
            p++;
        } while (i < 2);
    }
}
