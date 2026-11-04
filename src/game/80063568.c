#include "game/types.h"

struct AnimEntry {
    u8 max;
    u8 cur;
    u8 type;
    u8 flags;
    s16 rate;
    s16 timer;
    u16 *src;
    u16 *dst;
};

struct AnimNode {
    struct AnimNode *next;
    s32 pad4;
    s32 count;
    struct AnimEntry entries[1];
};

extern struct AnimNode *D_8009FA04;
extern u8 D_800A2730[];

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern s32 func_80019668(void);

void func_80063568(s32 dt)
{
    struct AnimNode *node;
    s32 i;
    s32 changed;
    s32 val;
    s32 other;
    u32 flags;
    struct AnimEntry *e;
    u16 *src;
    u16 *dst;
    s32 off;

    for (node = D_8009FA04; node != 0; node = node->next) {
        e = node->entries;
        for (i = 0; i < node->count; i++, e++) {
            flags = e->flags;
            if (flags & 4) {
                continue;
            }
            changed = 0;
            if (e->type < 16) {
                s32 r;
                s32 t = (u16)e->timer - dt;
                e->timer = t;
                if ((s16)t <= 0) {
                    changed = 1;
                    r = (u16)e->rate;
                    e->timer = t + r;
                    switch (e->type) {
                    case 0:
                        e->cur++;
                        if (e->cur == e->max) {
                            e->cur = 0;
                        }
                        break;
                    case 2:
                        if (flags & 2) {
                            if (e->cur == 0) {
                                e->cur = e->cur + 1;
                                e->flags = flags ^ 2;
                            } else {
                                e->cur = e->cur - 1;
                            }
                        } else {
                            e->cur++;
                            if (e->cur == e->max) {
                                e->cur = e->max - 2;
                                e->flags = flags ^ 2;
                            }
                        }
                        break;
                    case 1:
                        if (e->cur == 0) {
                            e->cur = e->max;
                        }
                        e->cur = e->cur - 1;
                        break;
                    case 3:
                        e->cur = (u32)func_80019668() % e->max;
                        break;
                    }
                }
            } else {
                s32 angle;
                s32 m = dt * e->rate;
                m >>= 3;
                angle = e->timer;
                angle += m;
                angle &= 0x7FFF;
                e->timer = angle;
                if (e->type == 0x11) {
                    func_8001846C(angle >> 3, &val, &other);
                } else {
                    func_8001846C((angle & 0x3FFF) >> 3, &other, &val);
                }
                val += 0x10000;
                val = (val * e->max) >> 17;
                if (!(val < e->max)) {
                    val = e->max - 1;
                }
                if (val != e->cur) {
                    e->cur = val;
                    changed = 1;
                }
            }
            if (changed) {
                src = e->src;
                if (flags & 1) {
                    off = e->cur << 3;
                } else {
                    off = e->cur * 6;
                }
                src = (u16 *)((u8 *)src + off);
                dst = e->dst;
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                if (flags & 1) {
                    dst[3] = src[3];
                }
            }
        }
    }

    if (D_800A2730[0x201] != 0) {
        u8 *p;
        u8 *q;

        dt += D_800A2730[0x200];
        dt = dt / 4;
        D_800A2730[0x200] = dt & 3;
        p = D_800A2730;
        q = D_800A2730 + 0x100;
        for (i = 0x40; i > 0; i--) {
            s32 b;
            s32 c;
            s32 d;
            s32 a;
            a = p[0];
            b = p[1];
            c = p[2];
            d = p[3];
            a -= dt;
            b -= dt;
            c -= dt;
            d -= dt;
            if (a < 0) {
                a = func_80019668() & 0x3F;
                a += 0x40;
                q[0] = a;
                a <<= 1;
            }
            if (b < 0) {
                b = func_80019668() & 0x3F;
                b += 0x40;
                q[1] = b;
                b <<= 1;
            }
            if (c < 0) {
                c = func_80019668() & 0x3F;
                c += 0x40;
                q[2] = c;
                c <<= 1;
            }
            if (d < 0) {
                d = func_80019668() & 0x3F;
                d += 0x40;
                q[3] = d;
                d <<= 1;
            }
            q += 4;
            p[0] = a;
            p[1] = b;
            p[2] = c;
            p[3] = d;
            p += 4;
        }
    }
}
