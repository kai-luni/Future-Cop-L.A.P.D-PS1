#include "game/types.h"

struct Unk58210U {
    u8 unk00[0x1C];
    u16 unk1C;
};

struct Unk58210Sub {
    u8 unk00[0x1C];
    u16 unk1C;
    u8 unk1E[0x84];
    s16 unkA2;
    u8 unkA4[0x1D];
    s8 unkC1;
    u8 unkC2[0x264];
    s16 unk326;
};

struct Unk58210 {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 unk21[3];
    u32 unk24;
    u8 unk28[0x84];
    struct Unk58210Sub *unkAC;
    u8 unkB0[0x2C];
    struct Unk58210Sub *unkDC;
};

struct Unk58210Global {
    u8 unk00[0x5C];
    s32 unk5C;
};

extern u8 D_8009FA8C[];
extern struct Unk58210Global *D_8009FA94;

extern void func_80057358(void *object, s32 amount);
extern void func_8003BA50(void *object, s32 value);

void func_80058210(struct Unk58210 *object, s32 amount)
{
    u8 *table;
    u8 *entry;
    u32 index;
    u16 health;
    s32 oldHealth;
    s32 value;
    s32 max;

    health = ((struct Unk58210U *)object)->unk1C;
    __asm__("" ::: "memory");
    oldHealth = object->unk1C;

    if (amount <= 0) {
        return;
    }

    __asm__("lui\t%0,%%hi(D_8009FA8C)\n\taddiu\t%0,%0,%%lo(D_8009FA8C)"
            : "=r"(table));
    index = object->unkAC->unkC1;
    entry = (u8 *)(index + (u32)table);
    if (*entry != 0) {
        return;
    }

    if (object->unkAC->unk326 > 0) {
        return;
    }

    object->unk1C = health - amount;
    object->unkDC->unk1C = object->unk1C;

    value = object->unkAC->unkA2 + ((amount * 5) << 10) / object->unk1E;

    if (value >= 0) {
        if (value < 0x1C3) {
            object->unkAC->unkA2 = value;
        } else {
            object->unkAC->unkA2 = 0x1C2;
        }
    } else {
        object->unkAC->unkA2 = 0;
    }

    if (value >= 0) {
        if (value < 0x201) {
            max = value;
        } else {
            max = 0x200;
        }
    } else {
        max = 0;
    }

    if (D_8009FA94->unk5C != 0) {
        max /= 2;
    }

    func_80057358(object, max);

    if (object->unk20 != 0 && (object->unk24 & 0x40000) == 0) {
        func_8003BA50(object, oldHealth);
    }
}
