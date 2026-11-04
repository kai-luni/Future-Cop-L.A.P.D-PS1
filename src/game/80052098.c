#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Coord {
    u8 pad00[0x34];
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[0x4];
    s16 unk44;
    s16 unk46;
};

struct Entity {
    u8 pad00[0x8];
    s32 id;
    u8 pad0C[0x0E];
    s16 unk1A;
    u8 pad1C[0x8];
    s32 flags;
};

extern s32 func_80061CAC(struct Coord *coordA, struct Coord *coordB);
extern void func_80058378(void *self, s32 amount, s32 source);
extern void func_80051EB8(void *self, struct Coord *coordB, s32 ret, s32 flags);

s32 func_80052098(void *self, struct Coord *coordA, struct Coord *coordB, struct Entity *other)
{
    s32 flags;
    s32 dz;
    s32 dx;
    s32 dy;
    s32 sum;
    s32 ret;
    s32 ax;

    flags = other->flags;
    dz = coordA->unk3C - coordB->unk3C;
    dy = coordA->unk38 - coordB->unk38;
    dx = coordA->unk34 - coordB->unk34;

    if (dz < 0) {
        dz = -dz;
    }
    ax = dx;
    if (ax < 0) {
        ax = -ax;
    }
    if (dz < ax) {
        dz ^= ax;
        ax ^= dz;
        dz ^= ax;
    }
    sum = dz + ax;
    if ((dz >> 1) < ax) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + dz + (ax << 5);
        ax = dz - ax;
        dz = sum >> 7;
    }
    if (dz + (ax >> 2) - (ax >> 6) - ((coordA->unk46 + coordB->unk46) << 4) >= 0) {
        return 0;
    }

    if (ABS(dy) - ((coordA->unk44 + coordB->unk44) << 4) >= 0) {
        return 0;
    }

    ret = func_80061CAC(coordA, coordB);
    if (ret == 0) {
        return 0;
    }

    func_80058378(self, other->unk1A, other->id);
    func_80051EB8(self, coordB, ret, flags);

    return 1;
}
