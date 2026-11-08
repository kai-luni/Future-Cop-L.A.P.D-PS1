#include "game/types.h"

struct UnkE720Sub {
    u8 pad[0x48];
    s16 unk48;
};

struct UnkE720 {
    u8 pad00[0x20];
    struct UnkE720Sub *unk20;
    u8 pad24[0x62];
    s16 unk86;
    u8 pad88;
    u8 unk89;
    u8 unk8A;
    u8 unk8B;
    u8 pad8C;
    u8 unk8D;
    u8 unk8E;
    u8 unk8F;
};

void func_8006E720(struct UnkE720 *self)
{
    s32 a2;
    s32 v1;
    s32 t;
    s32 d;
    s32 h;
    s32 p;

    if (self->unk86 < 0x10) {
        self->unk86 = 0;
        return;
    }
    a2 = self->unk89 > self->unk8A ? self->unk89 : self->unk8A;
    if (self->unk8B > a2) {
        a2 = self->unk8B;
    }
    v1 = self->unk8D > self->unk8E ? self->unk8D : self->unk8E;
    if (self->unk8F > v1) {
        v1 = self->unk8F;
    }
    if (v1 > a2) {
        a2 = v1;
    }
    t = self->unk20->unk48;
    h = t << 4;
    p = t << 6;
    d = (p + h) * 30;
    self->unk86 = (s16)((((0x80000000u / (u32)d) >> 4) * a2) >> 4);
}
