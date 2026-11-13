#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Mat {
    s32 m[9];
};

struct Xform {
    struct Mat m;
    struct Vec3s32 pos;
};

struct Pool {
    void *storage;
    struct Particle *active;
    struct Particle *free;
};

struct Particle {
    struct Particle *next;
    struct Vec3s32 pos;
    u16 unk10;
    s8 life;
    u8 pad13;
    u8 idx;
    u8 r;
    u8 g;
    u8 b;
    u16 flags;
    u16 pad1A;
    s16 vx;
    s16 vy;
    s16 vz;
    u8 pad22[6];
    s8 unk28;
    s8 unk29;
    u8 unk2A;
    u8 pad2B;
    u16 unk2C;
    u16 unk2E;
};

struct Anim {
    s8 base;
    s8 count;
    u8 cur;
    u8 pad;
    u16 reload;
    u16 timer;
};

struct Sub {
    u8 pad[0x34];
    void *child;
};

struct Emitter {
    struct Emitter *next;
    u8 pad04[0xA];
    s8 part;
    u8 flagsF;
    u8 pad10[0xA];
    u16 flags1A;
    struct Sub *sub;
    u8 pad20[8];
    struct Vec3s32 pos;
    struct Mat mat;
    u8 pad58[0xC];
    struct Pool pool;
    s16 scale;
    u8 pad72;
    u8 c0[3];
    u8 c1[3];
    u8 c2[3];
    s16 a[3];
    s16 b[3];
    s16 c[3];
    u8 pad8E_[0];
    u16 flags8E;
    s16 limit;
    u8 pad92[4];
    s16 unk96;
    s16 unk98;
};

extern void func_8001006C(void *src, void *dst, s32 count);
extern void func_80010820(void *m);
extern void func_80017668(void *inst, s32 part, s32 *out);
extern s32 func_80019F54(s32 *v);
extern void func_8001A81C(void *m, void *in, void *out);
extern void *func_8003BEF0(void *node);
extern void func_8003BF1C(void *pool, void *prev, void *node);
extern struct Xform *func_8003D9C4(struct Emitter *e);
extern void func_8003D9F8(void *a, s32 b, s32 c, s32 d, void *e, void *f);
extern void func_800753D0(struct Particle *p);
extern void func_80075478(void *p);
extern void func_800754B8(struct Emitter *e, void *node, void *vec, struct Xform *x);
extern s32 func_80066F78(void *a, s32 b, s32 c, s32 d);
extern u8 func_800751A4(struct Particle *p, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);

void func_80075A84(struct Emitter *e, s32 step)
{
    s32 tmp[3];
    s32 vec[3];
    struct Xform local;
    s16 sc;
    struct Xform *lp;
    struct Xform *xf;
    struct Emitter *self;
    s32 shift;
    s32 prevType;
    struct Particle *prev;
    s32 hit;
    struct Particle *p;
    struct Particle *next;

    shift = step << 4;
    if (e != 0) {
    lp = &local;
    do {
        if (!(e->flagsF & 8)) {
            sc = (e->scale * shift) >> 12;
            prevType = 3;
            prev = 0;
            hit = 0;
            self = e;
            if (e->pool.active == 0) {
                if (e->flags8E & 1) {
                    e->flagsF |= 1;
                }
                goto nextE;
            }
            xf = func_8003D9C4(e);
            if (e->flags8E & 0x10) {
                { s32 t0 = e->mat.m[0]; local.m.m[1] = e->mat.m[1]; local.m.m[2] = e->mat.m[2]; local.m.m[3] = e->mat.m[3]; local.m.m[4] = e->mat.m[4]; local.m.m[5] = e->mat.m[5]; local.m.m[6] = e->mat.m[6]; local.m.m[7] = e->mat.m[7]; local.m.m[8] = e->mat.m[8]; local.m.m[0] = t0; }
                local.pos = e->pos;
                xf = &local;
            } else if (!(e->flags1A & 0x100)) {
                struct Xform *o;
                void *child;
                child = e->sub ? e->sub->child : 0;
                if (child != 0 && e->part >= 0 && e->part < 4) {
                    func_80017668(child, e->part, (s32 *)&local.pos);
                    { s32 t0 = e->mat.m[0]; local.m.m[1] = e->mat.m[1]; local.m.m[2] = e->mat.m[2]; local.m.m[3] = e->mat.m[3]; local.m.m[4] = e->mat.m[4]; local.m.m[5] = e->mat.m[5]; local.m.m[6] = e->mat.m[6]; local.m.m[7] = e->mat.m[7]; local.m.m[8] = e->mat.m[8]; local.m.m[0] = t0; }
                    o = lp;
                } else {
                    o = xf;
                }
                self->pos.x = o->pos.x - xf->pos.x;
                self->pos.y = o->pos.y - xf->pos.y;
                self->pos.z = o->pos.z - xf->pos.z;
            } else {
                func_8003D9F8(e->sub ? e->sub->child : 0, e->flags1A, e->part, 1, e->sub, &e->mat);
            }
            if (self->flags1A & 0x100) {
                func_8001A81C(xf, self->pad58, vec);
            } else if (self->flags8E & 0x10) {
                *(struct Vec3s32 *)vec = self->pos;
            } else {
                func_80010820(xf);
                func_8001006C(&self->pos, vec, 1);
                vec[0] = self->pos.x + xf->pos.x;
                vec[1] = self->pos.y + xf->pos.y;
                vec[2] = self->pos.z + xf->pos.z;
            }
            tmp[0] = vec[0] - ((s32 *)self->pool.active)[1];
            tmp[1] = vec[1] - ((s32 *)self->pool.active)[2];
            tmp[2] = vec[2] - ((s32 *)self->pool.active)[3];
            {
                s16 cnt = func_80019F54(tmp) >> 4;
                if (!(self->flags8E & 0x2000)) {
                    while (cnt >= self->limit) {
                        struct Particle *np;
                        cnt -= self->limit;
                        np = func_8003BEF0(&self->pool);
                        if (np != 0) {
                            func_800754B8(self, np, vec, xf);
                        }
                    }
                }
            }
            p = self->pool.active;
            while (p != 0) {
                next = p->next;
                if (p->life > 0) {
                    p->life--;
                    if (p->life <= 0) {
                        p->flags |= 1;
                    }
                }
                if (p->flags & 1) {
                    hit = 1;
                }
                {
                struct Anim *an = (struct Anim *)&p->unk28;
                an->timer = p->unk2E - step;
                if ((s16)an->timer <= 0) {
                    an->cur = p->unk2A + 1;
                    if (!((s8)an->cur < p->unk28 + p->unk29)) {
                        p->unk2A = p->unk28;
                    }
                    p->unk2E = p->unk2C;
                }
                }
                if (p->idx < 3) {
                    s32 a1 = (self->a[p->idx] * step) >> 8;
                    s32 a2 = (self->b[p->idx] * step) >> 8;
                    s32 a3 = (self->c[p->idx] * step) >> 8;
                    s32 r;
                    if (p->idx == 2) {
                        r = !func_800751A4(p, a1, a2, a3, 0, 0, 0);
                    } else {
                        r = !func_800751A4(p, a1, a2, a3, self->c0[p->idx + 1],
                                          self->c1[p->idx + 1], self->c2[p->idx + 1]);
                    }
                    if ((u8)r) {
                        p->idx++;
                        if (p->idx < 3) {
                            p->r = self->c0[p->idx];
                            p->g = self->c1[p->idx];
                            p->b = self->c2[p->idx];
                        }
                    }
                }
                p->unk10 += sc;
                if (self->flags8E & 0x8000) {
                    if (self->unk96 != 0) {
                        p->vy += (self->unk96 * shift) >> 12;
                    }
                    if (self->unk98 != 0) {
                        p->vx -= (p->vx * self->unk98) >> 12;
                        p->vy -= (p->vy * self->unk98) >> 12;
                        p->vz -= (p->vz * self->unk98) >> 12;
                    }
                    p->pos.x += (p->vx * shift) >> 8;
                    p->pos.y += (p->vy * shift) >> 8;
                    p->pos.z += (p->vz * shift) >> 8;
                }
                if (self->flags8E & 2) {
                    func_800753D0(p);
                }
                if ((u8)hit || prevType + p->idx == 6) {
                    if (self->flags8E & 2) {
                        func_80075478(p);
                    }
                    func_8003BF1C(&self->pool, prev, p);
                    if (self->pool.active == 0 && (self->flags8E & 1)) {
                        self->flagsF |= 1;
                        goto nextE;
                    }
                } else {
                    if (self->flags8E & 8) {
                        s32 r2 = func_80066F78(&p->pos, 0, 0, 0);
                        prev = p;
                        if (p->pos.y < r2) {
                            p->flags |= 1;
                        }
                    }
                    prev = p;
                }
                prevType = p->idx;
                p = next;
            }
        }
nextE:
        e = e->next;
    } while (e != 0);
    }
}
