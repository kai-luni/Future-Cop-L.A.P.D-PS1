#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Unk8002FF6C {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x50 - 0x17];
    s32 x;
    s32 y;
    s32 z;
    u8 unk5C[0xB4 - 0x5C];
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    u8 unkC0[0xC8 - 0xC0];
    s32 unkC8;
    u8 unkCC[4];
    u8 unkD0[4];
};

extern void func_8002FC90(struct Unk8002FF6C *other, s32 arg1, s32 arg2,
                          u8 *arg3, u8 *arg4);
extern void func_8004D958(struct Unk8002FF6C *self);

s32 func_8002FF6C(struct Unk8002FF6C *self, struct Unk8002FF6C *other)
{
    s32 dx;
    s32 dy;
    s32 dz;

    if (other->unk16 == 0x12) {
        other = (struct Unk8002FF6C *)other->unkB4;
    }
    if (other->unk16 != 1) {
        return 0;
    }
    dx = self->x - other->x;
    dy = self->y - other->y;
    dz = self->z - other->z;
    if (ABS(dx) < self->unkB4) {
        if (ABS(dy) < self->unkB8) {
            if (ABS(dz) < self->unkBC) {
                func_8002FC90(other, self->unkC8, -1, self->unkD0, self->unkCC);
                if (self->unkC8 & 0x2000000) {
                    func_8004D958(self);
                }
            }
        }
    }
    return 0;
}
