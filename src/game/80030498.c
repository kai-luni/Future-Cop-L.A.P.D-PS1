#include "game/types.h"

struct Coord {
    u8 pad00[0x34];
    s32 unk34;
    s32 unk38;
    s32 unk3C;
};

struct OtherCoord {
    u8 pad00[0x34];
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
};

struct Entity {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[0x11];
    struct Coord coord;
    s32 unk68;
    u8 pad6C[0x34];
    u8 unkA0[0x14];
    struct Entity *unkB4;
    u8 padB8[0x05];
    u8 unkBD;
    u8 padBE[0x08];
    s16 unkC6;
    u8 padC8[0x04];
    s16 unkCC;
    u8 padCE[0x02];
    struct Entity *unkD0;
};

struct Other {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[0x11];
    struct OtherCoord coord;
    u8 pad6C[0x48];
    struct Other *unkB4;
};

extern void func_800448D8(void *track, s32 index, s32 direction, s32 flags);

void func_80030498(struct Entity *self, struct Other *other)
{
    struct Coord *a;
    struct OtherCoord *b;
    s32 dz;
    s32 dx;
    s32 ax;
    s32 sum;
    s32 dist;
    s32 limit;

    if (self->unkBD != 0 && self->unkBD != 2) {
        return;
    }

    a = &self->coord;
    b = &other->coord;

    dz = a->unk3C - b->unk3C;
    dx = a->unk34 - b->unk34;
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
    dist = dz + (ax >> 2) - (ax >> 6);

    if (self->unkBD == 0) {
        limit = (self->unkC6 << 4) + b->unk40 + 0x4000;
    } else {
        limit = self->unk68 + b->unk40;
    }

    if (dist < limit) {
        dz = dist;
        __asm__("" : "+r"(dz), "+r"(dist));
        dx = a->unk38 - b->unk38;
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
        if (dz + (ax >> 2) - (ax >> 6) < limit) {
            switch (self->unkBD) {
            case 0:
                func_800448D8((u8 *)self + 0xA0, 1, 1, 1);
                self->unkBD = 1;
                break;
            case 2:
                self->unkBD = 5;
                self->unkCC = 0x190;
                if (other->unk16 == 0x12) {
                    other = other->unkB4;
                }
                self->unkD0 = (struct Entity *)other;
                break;
            }
        }
    }
}
