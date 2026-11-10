#include "game/types.h"

struct Coord {
    u8 pad00[0x34];
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
};

struct Sub {
    u8 pad00[0x12];
    u16 unk12;
    u8 pad14[0x2C];
    s32 unk40;
    u8 pad44[0x7D];
    s8 unkC1;
};

struct Ent;

struct Trk {
    u8 pad00[0x12];
    u16 unk12;
    u8 pad14[4];
    u16 unk18;
    u8 pad1A[0x20];
    u16 unk3A;
};

struct Ent {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 pad17;
    u8 unk18;
    u8 pad19[0x0A];
    u8 unk23;
    u32 unk24;
    struct Coord coord;
    u8 pad6C[0x26];
    u8 unk92;
    u8 unk93;
    u8 pad94[8];
    u8 *unk9C;
    u8 pad_a0[6];
    u8 unkA6;
    u8 padA7;
    u16 unkA8;
    u8 padAA[2];
    struct Sub *unkAC;
    u8 padB0[4];
    struct Ent *unkB4;
    u8 padB8[0x24];
    u16 unkDC;
    u8 unkDE;
    u8 padDF[0x11];
    u8 unkF0[6];
    u8 unkF6;
    u8 unkF7;
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    s16 unkFC;
    u8 unkFE;
    u8 padFF[5];
    struct Ent *unk104;
    u8 pad108[0x2C];
    struct Trk *unk134;
};

struct Menu {
    u8 pad00[0x64];
    s32 unk64[2];
    u8 pad6C[0x10];
    s32 unk7C[2];
    u8 pad84[0x5C];
    s32 unkE0;
};

extern struct Ent *D_8009FA58[2];
extern s32 D_8009FA0C;
extern struct Ent *D_8009F9EC;
extern struct Menu *D_8009FA94;
extern void func_80069D68();

void func_8002D1A0(struct Ent *self, struct Ent *other)
{
    s32 i;
    register struct Ent **pp __asm__("$5");
    struct Coord *a;
    struct Coord *b;
    s32 dz, dx, ax, sum, dist, limit;
    s32 mode;
    s32 id;
    register s32 result __asm__("$2");

    if (self->unkFE != 0) return;
    if (other->unk18 != self->unkF6 && other->unk18 != self->unkF7) return;

    if (other->unk16 == 1 || other->unk16 == 0x12) {
        if (other->unk16 == 0x12) {
            other = other->unkB4;
            if (other == 0) return;
        }
        for (i = 0, pp = D_8009FA58; i < 2; i++, pp++) {
            struct Ent *e = *pp;
            if (e != 0 && other->unk14 == e->unk14 && e->unkAC->unk40 == 0) goto ok;
        }
        return;
    } else if (other->unk16 != 5 && other->unk16 != 0x25) {
        return;
    }
ok:
    a = &self->coord;
    b = &other->coord;
    dz = a->unk3C - b->unk3C;
    dx = a->unk34 - b->unk34;
    if (dz < 0) dz = -dz;
    ax = dx;
    if (ax < 0) ax = -ax;
    if (dz < ax) { dz ^= ax; ax ^= dz; dz ^= ax; }
    sum = dz + ax;
    if ((dz >> 1) < ax) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + dz + (ax << 5);
        ax = dz - ax;
        dz = sum >> 7;
    }
    dist = dz + (ax >> 2) - (ax >> 6);
    limit = (self->unkFC << 4) + b->unk40 + 0x4000;
    if (dist - limit >= 0) return;

    if (other->unk16 == 0x25) {
        result = dist;
        goto check_distance;
    }
    {
        dz = dist;
        __asm__("" : "+r"(dz), "+r"(dist));
        dx = a->unk38 - b->unk38;
        if (dz < 0) dz = -dz;
        ax = dx;
        if (ax < 0) ax = -ax;
        if (dz < ax) { dz ^= ax; ax ^= dz; dz ^= ax; }
        sum = dz + ax;
        if ((dz >> 1) < ax) {
            sum = (sum << 6) + (sum << 3) + (sum << 1) + dz + (ax << 5);
            ax = dz - ax;
            dz = sum >> 7;
        }
        result = dz + (ax >> 2) - (ax >> 6);
        __asm__("" : "+r"(result));
    }
check_distance:
    __asm__("" : "+r"(result));
    if (result - limit >= 0) return;

    id = other->unk18;
    mode = (id == self->unkF6) ? 1 : 2;
    self->unkFE = mode;
    self->unk18 = id;
    self->unk104->unk18 = id;
    self->unkA6 = 4;
    self->unkDE = 4;
    if (mode == 1) self->unkA8 = self->unkF7;
    else self->unkA8 = self->unkF6;
    self->unkDC = self->unkA8;
    self->unk23 = (mode == 1) ? self->unkF8 : self->unkF9;
    self->unk92 += self->unkFA * mode;
    self->unk104->unk92 = self->unk92;
    self->unk93 += self->unkFB * mode;
    self->unk104->unk93 = self->unk93;
    self->unk24 &= ~1;
    {
        struct Ent *e;
        i = 0;
        if (D_8009FA0C == 1 && (e = D_8009F9EC) != 0 && e->unk18 == id) {
            struct Menu *m = D_8009FA94;
            m->unk64[1]++;
            m->unk7C[1]++;
            e->unk134->unk3A++;
            e->unk134->unk18 = 0;
            if (other != 0 && (other->unk14 & 0x3FFF) == e->unk14) m->unkE0 = 6;
        } else {
            struct Menu *m = D_8009FA94;
            register struct Ent **p __asm__("$4");
            p = D_8009FA58;
            do {
                if (*p != 0 && id == (*p)->unk18) {
                    m->unk64[(*p)->unkAC->unkC1]++;
                    m->unk7C[(*p)->unkAC->unkC1]++;
                    (*p)->unkAC->unk12++;
                }
                i++;
                p++;
            } while (i < 2);
        }
    }
    if (self->unk9C != 0 && *self->unk9C != 0 && (self->unk24 & 0x100000))
        func_80069D68(self->unk9C, 0, other->unk14 & 0x7FFF);
}
