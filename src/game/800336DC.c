#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Ai {
    struct Vec3 posA;
    struct Vec3 posB;
    u8 pad18[2];
    u8 mode;
    u8 b1B;
    u8 c1C;
    u8 pad1D[0x13];
    u16 u30;
    u16 u32;
    u8 pad34[0x18];
    s32 s4C;
};

struct Aux {
    u8 pad[0x324];
    s16 a324;
    s16 a326;
};

struct Unit {
    struct Unit *next;
    u8 pad04[4];
    s32 active;
    u8 pad0C[0xA];
    u8 type;
    u8 pad17;
    u8 team;
    u8 sub;
    u8 pad1A[2];
    s16 s1C;
    u16 u1E;
    u8 pad20[0x30];
    struct Vec3 pos;
    u8 pad5C[0x4A];
    u8 aq6;
    u8 pada7;
    u8 pada8[2];
    s16 aa;
    struct Aux *aux;
    u8 padB0[8];
    void *b8;
    u8 padBC[0xC];
    void *c8;
    u8 padCC[0x2E];
    u8 fa;
    u8 padFB[0x39];
    struct Ai *ai;
};

struct Glob {
    u8 pad00[0x4C];
    s32 s4C;
    u8 pad50[0x18];
    s32 f68;
    u8 pad6C[8];
    s32 f74;
    s32 f78;
    u8 pad7C[8];
    s32 f84;
    s32 f88;
    u8 pad8C[0x10];
    s32 f9C;
    s32 a0[4];
    s32 fB0;
    s32 fB4;
    s32 fB8;
    s32 fBC;
    s32 c[4][2];
    s32 fE0;
};

extern struct Unit *D_8009FA40;
extern struct Glob *D_8009FA94;
extern struct Unit *D_8009F9EC;
extern s32 D_8009FAA4;
extern void *D_8009FA58;
extern u8 D_8009AC50[];
extern u8 D_8009AC54[];

extern s32 func_80019668(void);
extern void func_80033608(struct Unit *self, void *anim);
extern void func_800336A8(struct Unit *self, void *anim);
extern s32 func_80095900(s32 v);
extern void func_800330E8(struct Unit *self);

#define G D_8009FA94
#define GC(i, k) (((s32 *)((s32)((i) << 3) + (s32)G))[0x30 + (k)])

#define ABS(v)                           \
    do {                                 \
        s32 g_ = (v) >> 31;              \
        (v) = (g_ + (v)) ^ g_;           \
    } while (0)

#define DIST(d, dx, dz)                  \
    do {                                 \
        s32 az_;                         \
        register s32 m_ __asm__("$4");   \
        register s32 h_ __asm__("$2");   \
        d = (dx);                        \
        ABS(d);                          \
        az_ = (dz);                      \
        ABS(az_);                        \
        m_ = d;                          \
        if (az_ < d) m_ = az_;           \
        h_ = (u32)m_ >> 31;              \
        h_ = m_ + h_;                    \
        h_ = h_ >> 1;                    \
        d = d + (az_ - h_);              \
    } while (0)

#define ABSP(v)                          \
    do {                                 \
        register s32 g_ __asm__("$3");   \
        register s32 s_ __asm__("$2");   \
        g_ = (v) >> 31;                  \
        s_ = g_ + (v);                   \
        (v) = s_ ^ g_;                   \
    } while (0)

#define DISTO(out, dx, za, zb, mid)               \
    do {                                 \
        register s32 az_ __asm__("$5");  \
        register s32 m_ __asm__("$4");   \
        register s32 h_ __asm__("$2");   \
        register s32 zu_ __asm__("$3");  \
        register s32 zw_ __asm__("$2");  \
        dist = (dx);                     \
        ABSP(dist);                       \
        zu_ = (za); zw_ = (zb); az_ = zu_ - zw_;                      \
        ABSP(az_);                        \
        m_ = dist;                       \
        if (az_ < dist) m_ = az_;        \
        h_ = (u32)m_ >> 31;              \
        h_ = m_ + h_;                    \
        h_ = h_ >> 1;                    \
        h_ = az_ - h_;                   \
        mid                              \
        out = dist + h_;                 \
    } while (0)

#define DISTA(dx, dz, tt)                \
    do {                                 \
        s32 az_;                         \
        register s32 m_ __asm__("$4");   \
        dist = (dx);                     \
        ABS(dist);                       \
        az_ = (dz);                      \
        ABS(az_);                        \
        m_ = dist;                       \
        if (az_ < dist) m_ = az_;        \
        tt = (u32)m_ >> 31;              \
        tt = m_ + tt;                    \
        tt = tt >> 1;                    \
        tt = az_ - tt;                   \
    } while (0)

#define APPROACH(q_)                                                        \
    if (self->c8 != (q_) || self->b8 != (q_)) {                              \
        if (self->b8 != (q_)) {                                             \
            register s32 ex_ __asm__("$5");                                                            \
            register s32 ez_ __asm__("$4");                                                            \
            register s32 m1_ __asm__("$2"); s32 m2_;                                                  \
            register s32 sx_ __asm__("$3"); register s32 lt_ __asm__("$3");                                                            \
            ex_ = (q_)->pos.x;                                                                         \
            sx_ = self->pos.x;                                                                         \
            ex_ -= sx_;                                                                                \
            ez_ = (q_)->pos.z;                                                                         \
            ez_ -= self->pos.z;                                                                        \
            FMUL(m1_, ex_, ex_);                                           \
            FMUL(m2_, ez_, ez_);                                           \
            lt_ = (func_80095900(m1_ + m2_) << 2) < (self->aa << 4); if (lt_) {       \
                func_80033608(self, (q_));                                  \
            }                                                              \
        }                                                                  \
        if (self->c8 != (q_)) {                                             \
            func_800336A8(self, (q_));                                      \
        }                                                                  \
    }

extern int abs(int);

void func_800336DC(struct Unit *self)
{
    s32 c10;
    s32 d14, d18, d1C, d20, d24, d28, d2C, d30, d34, d38, d3C, d40, d44, d48;
    struct Unit *n4C;
    struct Unit *n50;
    struct Unit *n54;
    struct Unit *n58;
    struct Unit *n5C;
    struct Unit *n60;
    struct Unit *n64;
    struct Unit *p;
    s32 cntA;
    s32 cnt2;
    struct Unit *c5;
    struct Unit *c1C;
    struct Unit *c9b;
    struct Unit *c9a;
    struct Unit *c9bB;
    struct Unit *c9aB;
    struct Unit *c1;
    s16 mode;
    s32 dist;
    s32 s1;
    s32 i;
    s32 n;
    s32 t;
    register s32 tv __asm__("$2");
    s32 bb;
    register s32 q_ __asm__("$4");
    register struct Ai *ai_ __asm__("$4");
    register u8 c1C_ __asm__("$3");
    struct Glob *gp_;
    register s32 w_ __asm__("$3");
    register s32 lt3_ __asm__("$2");
    register s32 lt2_ __asm__("$3");

    cntA = 0;
    cnt2 = 0;
    c5 = 0;
    c1C = 0;
    c9bB = 0;
    c9aB = 0;
    c9b = 0;
    c9a = 0;
    c1 = 0;
    n50 = 0;
    n4C = 0;
    n58 = 0;
    n54 = 0;
    n5C = 0;
    n64 = 0;
    n60 = 0;

    for (p = D_8009FA40; p != 0; p = p->next) {
        if (p->active == 0) continue;
        switch (p->type) {
        case 1:
        case 5:
        case 6:
        case 8:
        case 9:
        case 28:
        case 36:
            if (p->team == self->ai->b1B
                || (p->type == 0x24 && p->team != self->team)
                || p->type == 8) {
                if (p->type == 0x1C || p->type == 5 || p->type == 9 || p->type == 8) {
                    DIST(dist, p->pos.x - self->ai->posB.x, p->pos.z - self->ai->posB.z);
                    switch (p->type) {
                    case 0x1C:
                        if (dist <= 0x90000) {
                            if (c1C == 0 || dist < d18) { c1C = p; d18 = dist; }
                        }
                        break;
                    case 5:
                        if (dist <= 0x80000) {
                            if (c5 == 0 || dist < d14) { c5 = p; d14 = dist; }
                        }
                        break;
                    case 9:
                        if (p->sub >= 10) {
                            if (c9b == 0 || dist < d30) { c9b = p; d30 = dist; }
                        } else {
                            if (c9a == 0 || dist < d2C) { c9a = p; d2C = dist; }
                        }
                        break;
                    case 8:
                        if (p->team == self->team && dist <= 0x1FFFFF) cntA++;
                        break;
                    }
                }
                DISTA(p->pos.x - self->pos.x, p->pos.z - self->pos.z, tv);
                bb = (9 - G->s4C) << 12;
                dist += tv;
                dist += bb;
                switch (p->type) {
                case 1:
                    if (c1 == 0 || dist < d48) {
                        if (p->aux->a324 <= 0 && p->aux->a326 <= 0) { c1 = p; d48 = dist; }
                    }
                    break;
                case 0x1C:
                    if (n54 == 0 || dist < d20) { n54 = p; d20 = dist; }
                    break;
                case 5:
                    if (n4C == 0 || dist < d1C) { n4C = p; d1C = dist; }
                    break;
                case 9:
                    if (p->sub >= 10) {
                        if (c9bB == 0 || dist < d38) { c9bB = p; d38 = dist; }
                    } else {
                        if (c9aB == 0 || dist < d34) { c9aB = p; d34 = dist; }
                    }
                    break;
                default:
                    if (p->type == 0x24) {
                        if (p->team == self->ai->b1B) {
                            if (n60 == 0 || dist < d40) { n60 = p; d40 = dist; }
                        } else if (p->team != self->team) {
                            d44 += func_80019668() & 0x3FFF;
                            if (n64 == 0 || dist < d44) { n64 = p; d44 = dist; }
                        }
                    }
                    break;
                }
            } else {
                if (p->team == self->team) {
                    DIST(dist, p->pos.x - self->ai->posA.x, p->pos.z - self->ai->posA.z);
                    switch (p->type) {
                    case 5:
                        if (dist <= 0x68000) {
                            if (n50 == 0 || dist < d24) { cnt2++; n50 = p; d24 = dist; }
                        }
                        break;
                    case 0x1C:
                        if (dist <= 0x78000) {
                            if (n58 == 0 || dist < d28) { cnt2 += 10; n58 = p; d28 = dist; }
                        }
                        break;
                    case 9:
                        if (p->sub >= 10) {
                            if (n5C == 0 || dist < d3C) { n5C = p; d3C = dist; }
                        }
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    mode = self->ai->mode;
    if (mode != 2) {
        if (c1C != 0 || c5 != 0 || (cntA < 10 && mode != 1)) {
            self->ai->mode = 2;
            G->f9C = 2;
        }
        if (self->ai->mode != 2) goto L402C;
    }
    if (c1C != 0) {
        if (c5 != 0 && d14 < d18) goto L_c5;
        APPROACH(c1C)
    } else if (c5 != 0) {
    L_c5:
        APPROACH(c5)
    } else if (cntA < 10) {
        if (self->c8 != D_8009FA58) func_800336A8(self, D_8009FA58);
    } else {
        self->ai->mode = 0;
        G->f9C = 0;
    }
    if (self->ai->mode == 2) goto L4170;
L402C:
    if (self->ai->mode != 1) {
        if (cnt2 >= 2) {
            self->ai->mode = 1;
            G->f9C = 1;
        }
        if (self->ai->mode != 1) goto L4170;
    }
    if (n58 != 0 && d28 <= 0x77FFF) {
        if (self->c8 != n58) {
            if (self->b8 == 0 && n64 != 0 && d44 <= 0xFFFF) {
                if (self->c8 != n64) func_800336A8(self, n64);
            } else {
                func_800336A8(self, n58);
            }
        }
    } else if (n50 != 0 && d24 <= 0x67FFF) {
        if (self->c8 != n50) {
            func_80033608(self, 0);
            func_800336A8(self, n50);
        }
    } else {
        self->ai->mode = 0;
        G->f9C = 0;
    }
L4170:
    if (self->ai->mode == 0) {
        if (c1 != 0 && d48 <= 0x4FFFF && c1->s1C < ((s16)c1->u1E >> 2)) {
            if (self->c8 != c1 || self->b8 != c1) {
                if (self->b8 != c1) {
                    register s32 ex_ __asm__("$5");
                    register s32 ez_ __asm__("$4");
                    register s32 m1_ __asm__("$2"); s32 m2_;
                    register s32 sx_ __asm__("$3"); register s32 lt_ __asm__("$3");
                    ex_ = c1->pos.x;
                    sx_ = self->pos.x;
                    ex_ -= sx_;
                    ez_ = c1->pos.z;
                    ez_ -= self->pos.z;
                    FMUL(m1_, ex_, ex_);
                    FMUL(m2_, ez_, ez_);
                    lt_ = (func_80095900(m1_ + m2_) << 2) < (self->aa << 4); if (lt_) {
                        func_80033608(self, c1);
                    }
                }
                if (self->c8 != c1) {
                    func_800336A8(self, c1);
                    G->fE0 = 3;
                }
            }
        } else if (n54 != 0 && d20 <= 0x4FFFF) {
            APPROACH(n54)
        } else if (n4C != 0 && d1C <= 0x3FFFF) {
            APPROACH(n4C)
        } else if (n64 != 0 && d44 <= 0x5FFFF) {
        L_n64:
            if (self->c8 != n64) func_800336A8(self, n64);
        } else if (c9aB != 0 && d34 <= 0x2FFFF) {
            APPROACH(c9aB)
        } else if (n60 != 0 && d40 <= 0x4FFFF) {
        L_n60:
            if (self->c8 != n60) func_800336A8(self, n60);
        } else if (c9bB != 0 && d38 <= 0x2FFFF) {
            if (self->c8 != c9bB || self->b8 != c9bB) {
                if (self->b8 != c9bB) {
                    register s32 ex_ __asm__("$5");
                    register s32 ez_ __asm__("$4");
                    register s32 m1_ __asm__("$2"); s32 m2_;
                    register s32 sx_ __asm__("$3"); register s32 lt_ __asm__("$3");
                    ex_ = c9bB->pos.x;
                    sx_ = self->pos.x;
                    ex_ -= sx_;
                    ez_ = c9bB->pos.z;
                    ez_ -= self->pos.z;
                    FMUL(m1_, ex_, ex_);
                    FMUL(m2_, ez_, ez_);
                    func_80095900(m1_ + m2_);
                    if (d38 < (self->aa << 4)) {
                        func_80033608(self, c9bB);
                    }
                }
                if (self->c8 != c9bB) {
                    func_800336A8(self, c9bB);
                }
            }
        } else if (n64 != 0) {
            goto L_n64;
        } else if (n60 != 0) {
            goto L_n60;
        } else {
            if (self->fa != 1) {
                self->fa = 1;
                func_800330E8(self);
            }
        }
        if (c1 != 0 && self->b8 != c1 && (self->b8 == 0 || (self->b8 != n54 && self->b8 != n4C))) {
            register s32 ex_ __asm__("$5");
            register s32 ez_ __asm__("$4");
            register s32 m1_ __asm__("$2"); s32 m2_;
            register s32 sx_ __asm__("$3"); register s32 lt_ __asm__("$3");
            ex_ = c1->pos.x;
            sx_ = self->pos.x;
            ex_ -= sx_;
            ez_ = c1->pos.z;
            ez_ -= self->pos.z;
            FMUL(m1_, ex_, ex_);
            FMUL(m2_, ez_, ez_);
            lt_ = (func_80095900(m1_ + m2_) << 2) < (self->aa << 4); if (lt_) {
                func_80033608(self, c1);
                G->fE0 = 2;
            }
        }
    }
    if (mode != self->ai->mode) func_800330E8(self);
    if (self->b8 == 0 && self->aq6 != 4) func_80033608(self, 0);

    if (self->ai->mode == 2) {
        if (G->s4C >= 9
            && ((c1C != 0 && d18 <= 0x23FFF) || (c5 != 0 && d14 <= 0x1FFFF))) {
            if (G->f74 < 0x14 && G->f68 > 0) {
                G->fB0 += 1;
                G->f68 -= 1;
                G->f74 += 1;
            }
        }
        DISTO(s1, self->pos.x - self->ai->posB.x, self->pos.z, self->ai->posB.z, __asm__ volatile(""); gp_ = G; __asm__ volatile(""););
        __asm__ volatile("");
        w_ = gp_->s4C;
        q_ = gp_->f78;
        s1 -= w_ << 16;
        lt2_ = q_ < (w_ >> 2) + 4;
        if (lt2_) {
            if (s1 <= 0x7FFFF) {
                { s32 r_ = (func_80019668() & 3) + 4;
                n = (G->s4C >> 2) + r_; }
                t = 0x14 - G->f78;
                if (t < n) n = t;
                t = G->f68;
                if (t < n) n = t;
                n -= (2 - (G->s4C >> 2)) * 2;
                i = 2;
                if (n > 0) {
                    G->fB4 += n;
                    G->f68 -= n;
                    G->f78 += n;
                }
                goto L49E4;
            } else if ((lt3_ = q_ < 10)) {
                i = 2;
            L49E4:
                do {
                    if (G->a0[i] == self->team) {
                        n = func_80019668() & 3;
                        t = 0x14 - G->f78;
                        if (t < n) n = t;
                        t = G->f68;
                        t -= 5;
                        if (t < n * 2) n = t / 2;
                        if (n > 0) {
                            GC(i, 1) += n;
                            G->f68 -= n * 2;
                            G->f78 += n;
                        }
                    }
                    i++;
                } while (i < 4);
            }
        }
        if (s1 <= 0x7FFFF && n5C == 0) {
            if (G->f68 > (u32)((func_80019668() & 7) + 0x41)) {
                G->f68 -= 0x32;
                G->fBC += 1;
            }
        }
    } else {
        i = 0;
        c10 = 0;
        do {
            s32 idx = (self->ai->c1C == 1 ? D_8009AC54 : D_8009AC50)[i];
            if (G->a0[idx] == 0) {
                if (G->f68 >= 0x1E) {
                    if (G->f88 == 0) D_8009F9EC->ai->s4C = D_8009FAA4;
                    G->a0[idx] = self->team;
                    G->f68 -= 0x1E;
                    if (G->f68 < 0x1E) break;
                } else {
                    c10++;
                }
            }
            i++;
        } while (i < 4);
    }

    if (self->ai->mode == 1) {
        for (i = 0; i < 2; i++) {
            if (G->a0[i] == self->team) {
                { s32 r_ = (func_80019668() & 7) + 5; n = r_ + (G->s4C >> 2); }
                t = 0x14 - G->f74;
                if (t < n) n = t;
                if (n >= 7 && G->f68 >= n * 2) {
                    GC(i, 0) += n;
                    G->f68 -= n * 2;
                    G->f74 += n;
                }
            }
        }
        if (G->f74 >= 12) {
            for (i = 0; i < 2; i++) {
                if (G->a0[i] == self->team) {
                    { s32 r_ = (func_80019668() & 3) + 2; n = r_ + (G->s4C >> 2); }
                    t = 0x14 - G->f78;
                    if (t < n) n = t;
                    if (n >= 4 && G->f68 >= n * 2) {
                        GC(i, 1) += n;
                        G->f68 -= n * 2;
                        G->f78 += n;
                    }
                }
            }
        }
    }

    if (self->ai->mode == 0) {
        if (n58 == 0) {
            if (G->f68 >= 0x32) {
                G->f68 -= 0x32;
                G->fB8 += 1;
            }
        }
        ai_ = self->ai;
        c1C_ = ai_->c1C;
        if ((c1C_ == 1 && G->s4C < 8)
            || (c1C_ == 0 && G->s4C < 6)
            || (c10 <= 0 && ai_->u32 > ai_->u30)) {
            if (G->s4C + 6 <= 0x14 - G->f74) {
                for (i = 0; i < 2; i++) {
                    if (G->a0[i] == self->team) {
                        n = (func_80019668() & 7) + G->s4C;
                        { s32 t2_ = n + 4;
                        n = t2_ + (G->s4C >> 2); }
                        t = 0x14 - G->f74;
                        if (t < n) n = t;
                        if (n >= G->s4C + 6 && G->f68 >= n * 2) {
                            GC(i, 0) += n;
                            G->f68 -= n * 2;
                            G->f74 += n;
                        }
                    }
                }
                if ((G->f88 == 0 && G->s4C < 5)
                    || (G->a0[0] == self->ai->b1B && G->a0[1] == G->a0[0])) {
                    if (G->f74 < 2) {
                        { s32 r_ = (func_80019668() & 3) + 6; n = G->s4C + r_; }
                        if (G->f84 == 4) n += 6;
                        t = 0x14 - G->f74;
                        if (t < n) n = t;
                        if (n >= G->s4C + 6 && G->f68 >= n) {
                            G->fB0 += n;
                            G->f68 -= n;
                            G->f74 += n;
                        }
                    }
                }
            }
        }
        if (self->ai->c1C == 0 && G->f78 <= 0) {
            q_ = (func_80019668() & 1) + 1;
            t = 0x14 - G->f78;
            if (t < q_) q_ = t;
            t = G->f68;
            if (t < q_) q_ = t;
            if (q_ > 0) {
                G->fB4 += q_;
                G->f68 -= q_;
                G->f78 += q_;
            }
        }
    }
}
