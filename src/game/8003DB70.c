#include "game/types.h"

struct Vec3 { s32 v[3]; };

struct Mtx {
    u8 pad[0x18];
    s32 t[3];
};

struct Obj {
    u8 unk00[4];
    u8 type;
    u8 unk05[0x13];
    s16 a;
    s16 b;
    s16 scale;
    s8 id;
    u8 unk1F[0x13];
    u16 flags;
    u16 v34;
    u16 v36;
    u8 unk38[4];
    u16 v3C;
    u16 v3E;
    u8 unk40[8];
    u16 v48;
    u16 v4A;
    u16 v4C;
    u8 unk4E[2];
    s8 v50;
};

struct Par {
    s32 w[13];
    s32 *unk34;
};

extern s32 D_8009AA38[9];
extern s32 func_80019668(void);
extern void func_8003D0F0(s32 *a, s32 b, s32 c, s32 *d);
extern void func_80010820(s32 *a);
extern void func_8001006C(void *a, void *b, s32 c);
extern void func_800188D0(s32 a, s32 b, s32 c, void *d);

static inline s32 rot_a(struct Obj *o, s32 *dst, struct Mtx *m)
{
    s16 a;
    s16 b;
    s32 result;
    s32 ta, tb;
    register u32 raw __asm__("$3");
    if (o->scale != 0) {
        ta = o->a;
        __asm__ volatile ("" : : "r"(ta) : "memory");
        raw = *(volatile u16 *)&o->a;
        __asm__ volatile ("" : "+r"(raw));
        a = raw;
        if (ta < 0) {
            a = (u32)func_80019668() % (u32)-o->a;
        }
        tb = o->b;
        b = *(volatile u16 *)&o->b;
        if (tb < 0) {
            b = (u32)func_80019668() % (u32)-o->b;
        }
        func_800188D0(a, -b, 0, m);
        __asm__ ("" : "+r"(dst));
        {
            s32 t, s;
            t = m->t[0]; s = o->scale;
            __asm__ volatile ("" : : "r"(t), "r"(s) : "memory");
            dst[0] = ((t >> 4) * s) >> 8;
            __asm__ volatile ("" : : : "memory");
            t = m->t[1]; s = o->scale;
            __asm__ volatile ("" : : "r"(t), "r"(s) : "memory");
            dst[1] = ((t >> 4) * s) >> 8;
            __asm__ volatile ("" : : : "memory");
            t = m->t[2]; s = o->scale;
            __asm__ volatile ("" : : "r"(t), "r"(s) : "memory");
            dst[2] = ((t >> 4) * s) >> 8;
        }
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

static inline s32 rot_b(struct Obj *o, void *m)
{
    s32 a;
    s32 b;
    switch (o->type) {
    case 0x5C:
        a = o->v3C;
        b = o->v3E;
        break;
    case 0x5E:
        a = o->v48;
        b = o->v4A;
        break;
    default:
        return 0;
    }
    if ((s16)a < 0) {
        register u32 remainder __asm__("$3");
        remainder = (u32)func_80019668() % (u32)-(s16)a;
        __asm__ volatile ("" : "+r"(remainder));
        a = remainder;
    }
    if ((s16)b < 0) {
        register u32 remainder __asm__("$3");
        remainder = (u32)func_80019668() % (u32)-(s16)b;
        __asm__ volatile ("" : "+r"(remainder));
        b = remainder;
    }
    if ((s16)a == 0 && (s16)b == 0x400) {
        return 0;
    }
    func_800188D0((s16)a, 0x400 - (s16)b, 0, m);
    return 1;
}

void func_8003DB70(struct Obj *o, s32 *x, struct Par *p_arg, u8 *y, s32 *out, s16 *o16, s8 *o8_arg)
{
    register struct Par *p __asm__("$21") = p_arg;
    register s8 *o8 __asm__("$8") = o8_arg;
    s32 v[3];
    struct Mtx m2;
    struct Mtx m1;
    s32 *s1;
    s32 a2;

    if (x != 0 && (o->flags & 8)) {
        out[0] = x[13];
        out[1] = x[14];
        out[2] = x[15];
        out[3] = x[16];
        out[4] = x[17];
        out[5] = x[18];
        out[6] = x[19];
        out[7] = x[20];
        out[8] = x[21];
        out[9] = x[22];
        out[10] = x[23];
        out[11] = x[24];
        *o16 = ((u16 *)x)[9];
        return;
    }
    if (y != 0) {
        if (y[0x49] & 1) {
            *o16 = *(s32 *)y >> 4;
        } else {
            switch (o->type) {
            case 0x5C:
                *o16 = o->v4C;
                break;
            case 0x5D:
                *o16 = o->v36;
                break;
            case 0x5E:
                *o16 = o->v50;
                break;
            case 0x57:
            case 0x58:
            case 0x59:
                *o16 = o->v34;
                break;
            default:
                *o16 = 0;
                break;
            }
        }
        if (y[0x49] & 0x20) {
            *o8 = y[0x48];
        } else {
            register s32 id __asm__("$4");
            register u8 raw __asm__("$3");
            id = o->id;
            raw = *(u8 *)&o->id;
            if (id != -1) {
                *o8 = raw;
            } else if (x != 0) {
                *o8 = ((u8 *)x)[0xE];
            } else {
                *o8 = id;
            }
        }
    } else {
        switch (o->type) {
        case 0x5C:
            *o16 = o->v4C;
            break;
        case 0x5D:
            *o16 = o->v36;
            break;
        case 0x5E:
            *o16 = o->v50;
            break;
        case 0x57:
        case 0x58:
        case 0x59:
            *o16 = o->v34;
            break;
        default:
            *o16 = 0;
            break;
        }
        *o8 = o->id;
    }

    if (*o16 == 0) {
        *o16 = ((s16 *)p)[0x1E];
    } else if (*o16 < 0) {
        if (p != 0) {
            *o16 = (((s16 *)p)[0x1E] * -*o16) >> 12;
        } else {
            *o16 = 0x100;
        }
    }

    if (p != 0) {
        s1 = p->unk34;
        if (s1 != 0) {
            s32 flags = o->flags;
            s32 id = *o8;
            __asm__ volatile ("" : : "r"(flags), "r"(id));
            a2 = 0;
            if (flags & 0x80) {
                register u32 bit __asm__("$2");
                bit = ((*(u32 **)s1)[3] >> 25) & 1;
                __asm__ ("" : "+r"(bit));
                a2 = bit < 1;
            }
            func_8003D0F0(s1, id, a2, out + 9);
            if (rot_a(o, v, &m1) & 0xFF) {
                out[9] += v[0];
                out[10] += v[1];
                out[11] += v[2];
            }
            if (!(o->flags & 0x100)) {
                if (o->flags & 4) {
                    func_80010820(s1 + 1);
                    func_8001006C(out + 9, out + 9, 1);
                    out[9] += s1[10];
                    out[10] += s1[11];
                    out[11] += s1[12];
                    if (rot_b(o, &m2) & 0xFF) {
                        func_80010820(p->unk34 + 1);
                        func_8001006C(&m2, out, 3);
                    } else {
                        out[0] = p->unk34[1];
                        out[1] = p->unk34[2];
                        out[2] = p->unk34[3];
                        out[3] = p->unk34[4];
                        out[4] = p->unk34[5];
                        out[5] = p->unk34[6];
                        out[6] = p->unk34[7];
                        out[7] = p->unk34[8];
                        out[8] = p->unk34[9];
                    }
                    return;
                }
                out[9] += s1[10];
                out[10] += s1[11];
                out[11] += s1[12];
            }
        } else {
            if (o != 0 && (o->flags & 1)) {
                goto L4D8;
            }
            if (o->flags & 0x100) {
                if (rot_a(o, out + 9, &m1) != 1) {
                    out[9] = 0;
                    out[10] = 0;
                    out[11] = 0;
                }
            } else {
                if (rot_a(o, v, &m1) & 0xFF) {
                    out[9] = p->w[10] + v[0];
                    out[10] = p->w[11] + v[1];
                    out[11] = p->w[12] + v[2];
                } else {
L4D8:
                {
                    s32 px = p->w[10];
                    s32 py = p->w[11];
                    s32 pz = p->w[12];
                    __asm__ volatile ("" : : "r"(px), "r"(py), "r"(pz) : "memory");
                    out[9] = px;
                    out[10] = py;
                    out[11] = pz;
                }
                }
            }
            if (o->flags & 0x104) {
                if (rot_b(o, &m2) & 0xFF) {
                    func_80010820(p->w + 1);
                    func_8001006C(&m2, out, 3);
                } else {
                    out[0] = p->w[1];
                    out[1] = p->w[2];
                    out[2] = p->w[3];
                    out[3] = p->w[4];
                    out[4] = p->w[5];
                    out[5] = p->w[6];
                    out[6] = p->w[7];
                    out[7] = p->w[8];
                    out[8] = p->w[9];
                }
                return;
            }
        }
    } else {
        goto LNull;
    }

LDefault:

    if (rot_b(o, out) != 1) {
        out[0] = D_8009AA38[0];
        out[1] = D_8009AA38[1];
        out[2] = D_8009AA38[2];
        out[3] = D_8009AA38[3];
        out[4] = D_8009AA38[4];
        out[5] = D_8009AA38[5];
        out[6] = D_8009AA38[6];
        out[7] = D_8009AA38[7];
        out[8] = D_8009AA38[8];
    }
    return;

LNull:
        if (y != 0 && (y[0x49] & 4)) {
            *(struct Vec3 *)(out + 9) = *(struct Vec3 *)(y + 0x10);
        } else {
            if (rot_a(o, out + 9, &m2) != 1) {
                out[9] = 0;
                out[10] = 0;
                out[11] = 0;
            }
        }
    goto LDefault;
}
