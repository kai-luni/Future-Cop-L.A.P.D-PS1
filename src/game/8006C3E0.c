#include "game/types.h"

struct Unk8006C3E0Other {
    u8 unk00[0x8];
    s32 unk08;
    u8 unk0C[0x18];
    u32 unk24;
};

struct Unk8006C3E0Inner {
    u8 unk00[0x68];
    s32 unk68;
    struct Unk8006C3E0Other *unk6C;
    u8 unk70[0x8];
    s32 unk78[3];
};

struct Unk8006C3E0Outer {
    u8 unk00[0xAC];
    struct Unk8006C3E0Inner *unkAC;
};

extern s32 func_8004E5F0(struct Unk8006C3E0Outer *arg0, struct Unk8006C3E0Other *arg1, s32 arg2, s32 arg3);

void func_8006C3E0(struct Unk8006C3E0Outer *arg0, s32 arg1)
{
    struct Unk8006C3E0Inner *inner;
    struct Unk8006C3E0Other *other;
    s32 i;
    s32 value;

    inner = arg0->unkAC;
    for (i = 0; i < 3; i++) {
        value = inner->unk78[i];
        if (value > 0) {
            value -= arg1;
            inner->unk78[i] = value;
            if (value < 0) {
                inner->unk78[i] = 0;
            }
        }
    }

    other = inner->unk6C;
    if (other != 0) {
        if ((other->unk24 & 0x20001) || other->unk08 != inner->unk68) {
            inner->unk6C = 0;
        } else if (func_8004E5F0(arg0, other, 0x18000, 0xE804) < 0) {
            inner->unk6C = 0;
        }
    }
}
