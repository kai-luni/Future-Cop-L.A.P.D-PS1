#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Msg {
    s32 a;
    u16 b;
    u8 c;
    u8 pad;
    void *d;
};

struct Stats {
    u8 unk00[0x84];
    s16 unk84[3];
    s16 unk8A[3];
};

struct Cand {
    u8 unk00[4];
    s32 (*handler)(s32, struct Cand *, struct Msg *, s32);
    s32 unk08;
    u8 unk0C[8];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19[0x0B];
    u32 flags;
    u8 unk28[4];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[6];
    s16 unk6E;
    u8 unk70[0x3C];
    struct Stats *stats;
};

struct Fx {
    struct Cand *unk00;
    struct Cand *unk04;
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
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s16 unk48;
    u16 unk4A;
    s16 unk4C;
    s16 unk4E;
    u16 unk50;
    u16 flags;
    s32 unk54;
    u8 unk58[0x150];
    u8 unk1A8[6];
    u8 unk1AE;
};

struct CandList {
    s32 count;
    s32 unk04;
    struct Cand *items[1];
};

extern s32 D_8009FAA4[];
extern struct CandList D_800A8F90;

extern void func_80017668(u8 *instance, s32 part, s32 *out);
extern s32 func_80019668(void);
extern s32 func_8001AB00(s32 *a, s32 *b);
extern u8 func_8005CED4(struct Fx *a, s32 *b, struct Cand *c);
extern void func_8005D060(struct Fx *a, u8 *b);
extern void func_8006C488(struct Cand *a);
extern void *func_80073500(s32 *pos, s32 a1, s32 a2, s32 a3);
extern void *func_80073590(struct Cand *owner, void *coord, s32 kind, s32 a3, s32 a4, s32 a5);
extern void func_8007377C(void *a, void *b, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7,
                          s32 a8, s32 a9, s32 a10, s32 a11, s32 a12, s32 a13);

u8 func_8005D8F4(struct Fx *self_, s32 a1)
{
    register s32 arg1 __asm__("$4");
    register struct Fx *self __asm__("$17");
    register struct Cand *cand;
    register s32 i;
    s8 picked;
    register u8 notified;
    register s32 scale;
    register s32 twice;
    register s32 tmp;
    s32 tick;
    s32 dx, dy, dz, r2;
    register s32 radius __asm__("$2");
    s32 delta[3];
    struct Msg msg;
    register s32 b __asm__("$4");
    s32 limit;
    register s32 dist __asm__("$2");
    void *nodeB;
    register u8 *byte __asm__("$4");
    s32 v48;
    s16 v4A;
    s16 v4C;
    u8 team;

    self = self_;
    arg1 = a1;
    if (self->unk04 != 0 && self->unk04->unk08 != self->unk08) {
        return 0;
    }
    notified = 1;
    if (self->flags & 1) {
    v48 = self->unk48;
    __asm__ volatile("" : : "r"(v48));
    v4A = self->unk4A;
    self->unk4A = self->unk4A + ((((self->unk4C - v48) * 2 - v4A * 8) * arg1) >> 8);
    self->unk48 = self->unk48 + self->unk4A;
    twice = v48 * 2;
    scale = (v48 * v48) >> 12;
    if (self->unk04->unk16 == 1) {
        self->unk18 = self->unk04->unk2C;
        self->unk1C = self->unk04->unk30;
        self->unk20 = self->unk04->unk34;
        self->unk24 = self->unk04->unk38;
        self->unk28 = self->unk04->unk3C;
        self->unk2C = self->unk04->unk40;
        self->unk30 = self->unk04->unk44;
        self->unk34 = self->unk04->unk48;
        self->unk38 = self->unk04->unk4C;
        self->unk3C = self->unk04->unk50;
        self->unk40 = self->unk04->unk54;
        self->unk44 = self->unk04->unk58;
        self->unk40 = self->unk40 + 0x1000;
    } else {
        self->unk18 = self->unk04->unk2C;
        self->unk1C = self->unk04->unk30;
        self->unk20 = self->unk04->unk34;
        self->unk24 = self->unk04->unk38;
        self->unk28 = self->unk04->unk3C;
        self->unk2C = self->unk04->unk40;
        self->unk30 = self->unk04->unk44;
        self->unk34 = self->unk04->unk48;
        self->unk38 = self->unk04->unk4C;
        self->unk3C = self->unk04->unk50;
        self->unk40 = self->unk04->unk54;
        self->unk44 = self->unk04->unk58;
        func_80017668((u8 *)self->unk04 + 0x28, 0, &self->unk3C);
    }
    i = 0;
    tmp = 0x58;
    while (i < 12) {
        func_8005D060(self, (u8 *)self + tmp);
        tmp += 0x1C;
        i++;
    }
    if (self->unk04->unk16 == 1) {
        __asm__ volatile("" : : : "$2");
        tick = (s32)0x800a0000;
        __asm__ volatile("" : "=r"(tick) : "0"(tick));
        tick = *(s32 *)(tick - 1372);
        __asm__ volatile("" : : "r"(tick));
        if (self->unk0C >= tick) goto collision_loop;
        {
            cand = self->unk04;
            if (self->unk14 < tick) {
                {
                    struct Stats *stat;
                    stat = cand->stats;
                    stat = (struct Stats *)((u8 *)stat + self->unk54 * 2);
                    __asm__ volatile("" : "=r"(stat) : "0"(stat));
                    tmp = stat->unk8A[0];
                }
                picked = tmp > 0;
                func_8006C488(cand);
                if (picked == 0) {
                    tmp = cand->stats->unk84[self->unk54];
                }
                if (tmp > 0) {
                    tmp = tmp - 1;
                    if (tmp <= 0) {
                        self->flags = self->flags & 0xFFFE;
                        tmp = 0;
                    }
                } else {
                    self->flags = self->flags & 0xFFFE;
                }
                if (picked != 0) {
                    {
                        struct Stats *stat;
                        s32 idx;
                        idx = self->unk54;
                        stat = cand->stats;
                        __asm__ volatile("" : "=r"(idx), "=r"(stat) : "0"(idx), "1"(stat));
                        stat = (struct Stats *)((u8 *)stat + idx * 2);
                        stat->unk8A[0] = tmp;
                    }
                } else {
                    cand->stats->unk84[self->unk54] = tmp;
                }
                self->unk14 = D_8009FAA4[0] + self->unk10;
            }
        }
    } else {
        __asm__ volatile("" : : : "$2");
        tick = (s32)0x800a0000;
        __asm__ volatile("" : "=r"(tick) : "0"(tick));
        tick = *(s32 *)(tick - 1372);
        if (tick > self->unk0C) {
            self->flags = self->flags & 0xFFFE;
        }
    }
    collision_loop:
    for (i = 0; i < D_800A8F90.count; i++) {
        cand = D_800A8F90.items[i];
        team = cand->unk18;
        if (cand->unk08 <= 0) {
            continue;
        }
        if (team == self->unk04->unk18) {
            continue;
        }
        if (cand->flags & 0x01000084) {
            continue;
        }
        limit = self->unk4E * 16;
        delta[0] = cand->unk5C - self->unk3C;
        delta[1] = cand->unk60 - self->unk40;
        delta[2] = cand->unk64 - self->unk44;
        if (ABS(delta[0]) >= limit) {
            continue;
        }
        if (ABS(delta[1]) >= limit) {
            continue;
        }
        if (ABS(delta[2]) >= limit) {
            continue;
        }
        if (self->unk00->unk18 & 4) {
            if (func_8001AB00(delta, &self->unk30) < 0) {
                continue;
            }
        }
        dx = (delta[0] >> 4) * (delta[0] >> 4);
        dy = (delta[1] >> 4) * (delta[1] >> 4);
        dz = (delta[2] >> 4) * (delta[2] >> 4);
        radius = cand->unk6E;
        r2 = (radius + twice) * radius;
        dist = dx >> 12;
        dist += dy >> 12;
        dist += dz >> 12;
        if (dist < scale + (r2 >> 12)) {
            self->unk1AE = 4;
            if (self->flags & 4) {
                *(s16 *)((u8 *)self + 0x4A) = -0x50;
            } else {
                *(s16 *)((u8 *)self + 0x4A) = -0x10;
            }
        } else {
            goto check;
        }
    send:
        msg.c = 8;
        if (cand->unk16 == 2 || cand->unk16 == 7 || cand->unk16 == 0xF) {
            if (self->flags & 4) {
                msg.b = 1000;
            } else {
                tmp = func_80019668();
                b = 0;
                if ((tmp & 7) == 7) {
                    b = 0x12C;
                }
                msg.b = b;
            }
        } else {
            msg.b = self->unk50;
        }
        msg.a = self->unk04->unk14;
        msg.d = self->unk04;
        cand->handler(0xE, cand, &msg, 0);
    check:
        if ((self->flags & 4) != 0 && notified != 0 &&
            func_8005CED4(self, delta, cand) != 0) {
            func_8007377C(func_80073500(delta, 0, 0x18, 0),
                          func_80073590(cand, 0, -1, 0, 0x18, 0), 0x20, 1, 0x7F, 0x7F, 0xFF, 0x2AA,
                          0x4000, -0x3000, 0, 0, 0, 1);
            goto send;
        }
        notified = 0;
    }
    if (notified != 0) {
        for (i = 0, byte = (u8 *)self; i < 12; i++) {
            byte[0x71] &= 0xFE;
            byte += 0x1C;
        }
    }
    } else {
        self->unk48 = 0;
        self->unk4A = 0;
    }
    return 1;
}
