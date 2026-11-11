#include "game/types.h"

struct Info {
    u8 pad00[8]; s32 count; u8 pad0c[0x1C];
    u8 instance[4]; s32 m[9]; u8 pad50[0x34];
    s32 limit; u8 pad88[0x84]; u16 h10C; u8 pad10E[10];
    u32 flags; u8 pad11C[0xE]; s16 yawBias;
};
struct Target { u8 pad00[8]; s32 id; u8 pad0c[0x50]; s32 pos[3]; };
struct Model { u8 pad00[0x10]; s32 anim; u8 pad14[0x20]; u8 *frames; };
struct Obj {
    u8 pad00[0x28]; struct Model *model; s32 m[9]; s32 pos[3]; s32 vtx[3];
    u8 pad68[8]; s16 frame; u8 pad72[6]; s32 old[3]; u8 pad84[0x1C];
    u8 script[6]; u8 padA6a; u8 padA6b[0x11]; struct Target *target; s32 targetId;
    u8 padC0[8]; u8 anim[0xC]; s16 animFrame; u8 padD6[6];
    s16 yaw; s16 pitch; s32 speed; s16 heading; u8 part; u8 mode; u16 flags;
    struct Info *info;
};

extern void func_80017668(void *, s32, void *);
extern s32 func_80095C30(s32, s32);
extern void func_800188D0(s32, s32, s32, void *);
extern void func_80010820(void *);
extern void func_8001006C(void *, void *, s32);
extern void func_8001846C(s32, s32 *, s32 *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern s32 func_80044B34(void *, void *, s32, s32);
extern void func_800175F8(void *, s32);
extern s32 func_80060238(void *, s32, void *, void *, s32);
extern void func_800448D8(void *, s32, s32, s32);
extern void func_8005F6E8(void *, void *, void *, s32);

union Prod { long long val; struct { u32 lo, hi; } w; };
#define FMUL(d,a,b) do { u32 raw; register u32 hi __asm__("$9"); register u32 lo __asm__("$8"); __asm__(".set reorder"); raw = (a) * (b); __asm__("" : "=l"(raw), "=h"(hi) : "0"(raw)); lo = raw; __asm__("" : : "r"(lo)); hi <<= 16; lo >>= 16; (d) = hi | lo; __asm__(".set noreorder"); } while (0)
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

#define STEP(field, tgt) \
    if (obj->field != (tgt)) { \
        s32 df = ((tgt) - obj->field) & 0xFFF; \
        s32 lim; \
        s32 ad; \
        if (df > 0x800) df -= 0x1000; \
        lim = dt * obj->speed; \
        ad = ABS(df); \
        if (ad < lim) lim = ad; \
        if (df < 0) lim = -lim; \
        obj->field = (obj->field + lim) & 0xFFF; \
    }

void func_80038A00(struct Obj *obj, s32 dt)
{
    struct Info *info;
    s32 d[3];
    s32 s1, s2;
    s32 f;
    s32 sc[2];

    info = obj->info;
    if (info->count <= 0) return;
    func_80017668(info->instance - 0, obj->part, obj->pos);
    f = obj->flags;
    if (!(f & 2)) {
        if (f & 0x20) {
            obj->yaw = obj->heading;
            goto apply;
        }
        s1 = 0;
        if (!obj->padA6a || obj->heading < 0) {
            s2 = (obj->yaw + 0x800) & 0xFFF;
        } else {
            struct Target *t = obj->target;
            if (f & 0x8000) {
                s2 = obj->yaw;
            } else if (t && t->id == obj->targetId) {
                d[0] = t->pos[0] - obj->vtx[0];
                d[1] = t->pos[1] - obj->vtx[1];
                d[2] = t->pos[2] - obj->vtx[2];
                s2 = func_80095C30(d[0], d[2]) & 0xFFF;
                if (!(obj->info->flags & 0x1000))
                    s2 = (s2 - info->yawBias) & 0xFFF;
                if (obj->flags & 0x10) {
                    register s32 a3 __asm__("$7");
                    register s32 a2 __asm__("$6");
                    s32 a0;
                    a3 = d[0];
                    if (a3 < 0) a3 = -a3;
                    a2 = d[2];
                    if (a2 < 0) a2 = -a2;
                    if (a3 < a2) { a3 ^= a2; a2 ^= a3; a3 ^= a2; }
                    if ((a3 >> 1) < a2) {
                        a0 = a3 + a2;
                        a0 = (a0 << 6) + (a0 << 3) + (a0 << 1) + a3 + (a2 << 5);
                        a2 = a3 - a2;
                        a3 = a0 >> 7;
                    }
                    {
                        register s32 q __asm__("$5");
                        register s32 r __asm__("$2");
                        register s32 arg0 __asm__("$4") = d[1];
                        __asm__("" : : "r"(arg0));
                        q = a2 >> 2;
                        q = a3 + q;
                        r = a2 >> 6;
                        q -= r;
                        s1 = (-(func_80095C30(arg0, q) << 12)) >> 12;
                    }
                    {
                        s32 cl;
                        if (s1 < -0x200) cl = -0x200;
                        else if (s1 < 0x201) cl = s1;
                        else cl = 0x200;
                        s1 = cl;
                    }
                }
            } else {
                s2 = obj->heading;
            }
        }
        STEP(pitch, s1);
        if (obj->yaw != s2) {
            if (obj->flags & 0x4000) {
                s32 h = obj->heading;
                register s32 tmp __asm__("$2") = s2 - h;
                register s32 df __asm__("$5") = tmp & 0xFFF;
                if (df > 0x800) df -= 0x1000;
                if (df < -0x300) s2 = (h - 0x300) & 0xFFF;
                else {
                    tmp = df < 0x301;
                    if (!tmp) s2 = (h + 0x300) & 0xFFF;
                }
            }
            STEP(yaw, s2);
        }
apply:
        func_800188D0(obj->yaw, obj->pitch, 0, obj->m);
        if (!(obj->info->flags & 0x1000)) {
            func_80010820(info->m);
            func_8001006C(obj->m, obj->m, 3);
        }
    } else {
        s32 h = (s16)info->h10C;
        s32 a0 = h >> 2;
        s32 v = h - a0;
        s32 v3 = info->limit;
        s1 = 0;
        if (v >= v3) {
            if (v3 < a0)
                s1 = 0x400;
            else
                s1 = 0x400 - ((v3 - a0) << 10) / (h >> 1);
        }
        STEP(pitch, s1);
        if (obj->pitch == 0) {
            obj->m[0] = info->m[0]; obj->m[1] = info->m[1]; obj->m[2] = info->m[2];
            obj->m[3] = info->m[3]; obj->m[4] = info->m[4]; obj->m[5] = info->m[5];
            obj->m[6] = info->m[6]; obj->m[7] = info->m[7]; obj->m[8] = info->m[8];
        } else {
            register s32 c __asm__("$6");
            register s32 s __asm__("$5");
            register s32 i3 __asm__("$7");
            register s32 i6 __asm__("$4");
            register s32 p3 __asm__("$3");
            register s32 p2 __asm__("$2");
            func_8001846C(obj->pitch, &sc[0], &sc[1]);
            {
                s32 t0 = info->m[0], t1 = info->m[1], t2 = info->m[2];
                obj->m[0] = t0; obj->m[1] = t1; obj->m[2] = t2;
            }
            i3 = info->m[3]; __asm__("" : : "r"(i3));
            c = sc[1]; __asm__("" : : "r"(c));
            FMUL(p3, c, i3);
            i6 = info->m[6]; __asm__("" : : "r"(i6));
            s = sc[0]; __asm__("" : : "r"(s));
            FMUL(p2, s, i6); p3 -= p2;
            FMUL(p2, c, i6); __asm__("" : : "r"(p2) : "memory"); obj->m[3] = p3;
            FMUL(p3, s, i3); p2 += p3; obj->m[6] = p2;
            i3 = info->m[4]; __asm__("" : : "r"(i3));
            FMUL(p2, c, i3);
            i6 = info->m[7]; __asm__("" : : "r"(i6));
            FMUL(p3, s, i6); p2 -= p3; obj->m[4] = p2;
            FMUL(p2, c, i6); FMUL(p3, s, i3); p2 += p3; obj->m[7] = p2;
            i3 = info->m[5]; __asm__("" : : "r"(i3));
            FMUL(p2, c, i3);
            i6 = info->m[8]; __asm__("" : : "r"(i6));
            FMUL(p3, s, i6); p2 -= p3; obj->m[5] = p2;
            FMUL(p2, c, i6); FMUL(p3, s, i3); p2 += p3; obj->m[8] = p2;
        }
    }
    func_80010750(obj->m);
    {
        register s32 *out __asm__("$5");
        register s32 one __asm__("$6");
        register s32 fo __asm__("$4");
        register struct Model *model __asm__("$2");
        register u8 *frames __asm__("$2");
        out = obj->vtx; one = 1;
        __asm__("" : : "r"(out), "r"(one));
        fo = obj->frame;
        __asm__("" : : "r"(fo));
        model = obj->model;
        __asm__("" : : "r"(model));
        fo = fo << 4;
        __asm__("" : "=r"(fo) : "0"(fo));
        frames = model->frames;
        __asm__("" : : "r"(frames));
        fo += 8;
        __asm__("" : : "r"(fo));
        func_80010234((u8 *)((s32)frames + fo), out, one);
    }
    obj->old[0] = obj->m[6]; obj->old[1] = obj->m[7]; obj->old[2] = obj->m[8];
    __asm__ volatile ("" : : : "memory");
    if (obj->model->anim) {
        s32 a = func_80044B34(obj, obj->anim, dt, 0) >> 1;
        while (a) {
            switch (a & 0x3F) {
            case 2: obj->flags |= 0x8000;
            case 1: if (obj->mode == 1) obj->mode = 2; break;
            case 3: obj->flags &= 0x7FFF; break;
            }
            a >>= 6;
        }
        func_800175F8(&obj->model, obj->animFrame);
    }
    f = obj->flags;
    if (f & 4) return;
    if (f & 8) {
        func_80060238(obj->script, dt, info, 0, 1);
        return;
    }
    if (obj->mode == 0) {
        if (f & 0x8000) return;
        if (func_80060238(obj->script, dt, obj, info, (((u32)f >> 7) ^ 1) & 1) & 0xFF) {
            if (obj->flags & 0x80) {
                obj->mode = 1;
                func_800448D8(obj->anim, 1, 1, 1);
            }
        }
    } else if (obj->mode == 2) {
        obj->mode = 0;
        func_8005F6E8(obj->script, obj, info, 1);
    }
}
