#include "game/types.h"

typedef struct V8 {
    s16 x, y, z, w;
} V8;

typedef struct Node {
    s16 idx;
    u8 pad2[6];
    u8 flags;
    u8 pad9[3];
    s32 c;
    s16 d;
    u8 pad12[2];
    s32 margin;
    V8 v[5];
} Node;

extern s32 *D_8009F9E4;
extern s32 D_8009FAA4;
typedef struct Pkt { u8 b[0x34]; } Pkt;
extern Pkt *D_8009F9F0;
extern Pkt *D_8009FA28;
extern u8 *D_8009F77C;
extern u8 *D_8009F778;

extern void func_8001010C(V8 *in, V8 *out, s32 n);
extern s32 func_80019D90(V8 *p, s32 m);
extern s32 func_80019E70(V8 *p, s32 m);
extern void func_80049F4C(V8 *p);
extern void func_8001846C(s32 angle, s32 *s, s32 *c);
extern void func_8001047C(V8 *a, V8 *b, u8 *c, s32 n);
extern s32 func_80049EF8(s32 a, s32 b, s32 c);

#define getTPage(tp, abr, x, y) \
    ((((tp) & 0x3) << 7) | (((abr) & 0x3) << 5) | (((y) & 0x100) >> 4) | (((x) & 0x3ff) >> 6) | (((y) & 0x200) << 2))
#define getClut(x, y) (((y) << 6) | (((x) >> 4) & 0x3f))
#define H(o)(*(s16 *)(s0 + (o)))
#define W(o) (*(s32 *)(s0 + (o)))

void func_8004A048(Node *node, s32 arg1, s32 arg2)
{
    V8 rot[5];
    V8 out[5];
    s16 d[3];
    u8 chk[4];
    u8 m;
    s32 sn, cs;
    s32 fl;
    u8 *s0;
    u8 *s3;
    u8 *s4;
    s32 s6;
    s32 s1, s2;
    s32 i;

    fl = node->flags & 0x30;
    d[2] = ((arg1 << 17) - D_8009F9E4[2]) >> 4;
    d[0] = ((arg2 << 17) - D_8009F9E4[0]) >> 4;
    d[1] = (-D_8009F9E4[1]) >> 4;

    if (node->flags & 4) {
        s32 a, b, p, q, t0, t1, t2, t3;
        out[0].x = node->v[0].x + d[0];
        out[0].y = node->v[0].y + d[1];
        out[0].z = node->v[0].z + d[2];
        func_8001010C(out, rot, 1);
        if ((func_80019D90(rot, node->margin) & 0xFF) != 2) {
            return;
        }
        if (node->idx < 0) {
            s3 = D_8009F77C + ((-node->idx) * 12 - 8);
            s4 = *(u8 **)(s3 + 8) + (((D_8009FAA4 - node->c) / node->d) % s3[0]) * 8;
        }
        *(s32 *)&rot[1] = *(s32 *)&node->v[1];
        func_80049F4C(rot);
        s0 = (u8 *)(D_8009F9F0 >= D_8009FA28 ? D_8009F9F0 : D_8009F9F0++);
        s6 = rot[0].z - 0x100;
        if (node->flags & 8) {
            s1 = rot[0].x >> 1;
            s2 = rot[0].y >> 1;
            func_8001846C(*(s16 *)&node->v[1].z, &sn, &cs);
            sn >>= 4;
            cs >>= 4;
            if (node->idx < 0) {
                a = rot[1].x;
                p = ((s4[4] * a) >> s3[1]) >> 1;
                t0 = p * cs;
                t1 = p * sn;
                q = ((s4[5] * a) >> s3[1]) >> 1;
                t2 = q * cs;
                t3 = q * sn;
            } else {
                p = rot[1].x;
                t0 = p * cs;
                t1 = p * sn;
                q = rot[1].y;
                t2 = q * cs;
                t3 = q * sn;
            }
            a = (t0 - t3) >> 13;
            b = (t0 + t3) >> 13;
            q = (t1 - t2) >> 13;
            p = (t1 + t2) >> 13;
            if (fl) {
                H(8) = s1 - a; H(0xA) = s2 - p;
                H(0x14) = s1 + b; H(0x16) = s2 + q;
                H(0x20) = s1 - b; H(0x22) = s2 - q;
                H(0x2C) = s1 + a; H(0x2E) = s2 + p;
            } else {
                H(8) = s1 - a; H(0xA) = s2 - p;
                H(0x10) = s1 + b; H(0x12) = s2 + q;
                H(0x18) = s1 - b; H(0x1A) = s2 - q;
                H(0x20) = s1 + a; H(0x22) = s2 + p;
            }
        } else {
            if (node->idx < 0) {
                t0 = ((rot[0].x - (rot[1].x >> 1)) + ((s4[2] * rot[1].x) >> s3[1])) >> 1;
                t1 = ((rot[0].x - (rot[1].x >> 1)) + ((s4[4] * rot[1].x) >> s3[1])) >> 1;
                q = ((rot[0].y - (rot[1].x >> 1)) + ((s4[3] * rot[1].x) >> s3[1])) >> 1;
                p = (rot[0].y - (rot[1].x >> 1)) + ((s4[5] * rot[1].x) >> s3[1]);
            } else {
                t0 = (rot[0].x - rot[1].x) >> 1;
                p = rot[0].y + rot[1].y;
                t1 = (rot[0].x + rot[1].x) >> 1;
                q = (rot[0].y - rot[1].y) >> 1;
            }
            p >>= 1;
            if (fl) {
                H(8) = t0; H(0xA) = q; H(0x14) = t1; H(0x16) = q;
                H(0x20) = t0; H(0x22) = p; H(0x2C) = t1; H(0x2E) = p;
            } else {
                H(8) = t0; H(0xA) = q; H(0x10) = t1; H(0x12) = q;
                H(0x18) = t0; H(0x1A) = p; H(0x20) = t1; H(0x22) = p;
            }
        }
    } else {
        for (i = 0; i < 5; i++) {
            out[i].x = node->v[i].x + d[0];
            out[i].y = node->v[i].y + d[1];
            out[i].z = node->v[i].z + d[2];
        }
        func_8001010C(out, rot, 1);
        if ((func_80019E70(rot, node->margin) & 0xFF) != 2) {
            return;
        }
        func_8001047C(&out[1], &rot[1], chk, 4);
        m = chk[0] | chk[1] | chk[2] | chk[3];
        if (m & 1) {
            return;
        }
        if (m != 0xC0) {
            return;
        }
        if (!(node->flags & 1)) {
            if (!func_80049EF8(*(s32 *)&rot[1], *(s32 *)&rot[2].x, *(s32 *)&rot[3].x)) {
                return;
            }
        }
        if (node->idx < 0) {
            s3 = D_8009F77C + ((-node->idx) * 12 - 8);
            s4 = *(u8 **)(s3 + 8) + (((D_8009FAA4 - node->c) / node->d) % s3[0]) * 8;
        }
        s0 = (u8 *)(D_8009F9F0 >= D_8009FA28 ? D_8009F9F0 : D_8009F9F0++);
        s6 = ((rot[0].z + rot[1].z + rot[2].z + rot[3].z) >> 2) - 0x100;
        if (fl) {
            W(8) = *(s32 *)&rot[1]; W(0x14) = *(s32 *)&rot[2];
            W(0x2C) = *(s32 *)&rot[3]; W(0x20) = *(s32 *)&rot[4];
        } else {
            W(8) = *(s32 *)&rot[1]; W(0x10) = *(s32 *)&rot[2];
            W(0x20) = *(s32 *)&rot[3]; W(0x18) = *(s32 *)&rot[4];
        }
    }

    {
        s32 a1, a2, a3, t0, t1, x, v0, v1, a0, b;
        u8 *q;
        u32 w;
        if (node->idx < 0) {
            a1 = s4[6]; a3 = s4[0]; a2 = s4[1];
            t1 = s4[4] - 1; t0 = s4[5] - 1;
        } else {
            q = D_8009F778 + (node->idx * 8 + 4);
            a1 = q[4]; a3 = q[0]; a2 = q[1];
            t1 = q[2] - 1; t0 = q[3] - 1;
        }
        if (node->flags & 0x10) {
            x = 1;
        } else {
            x = (node->flags >> 4) & 2;
        }
        /* Retail reads s4[7] on this path as well (0x8004A828), although s4 is only
           built in the idx < 0 branch above. The original therefore consumes a stale
           pointer here when node->idx >= 0; reproduced as-is for byte identity. A PC or
           Rust port must treat this read as potentially uninitialized and decide what the
           intended table entry is (the idx >= 0 path builds q, not s4). */
        if (a1 < 10) {
            a0 = getTPage(1, x, ((a1 >> 1) << 7) + 0x180, (a1 & 1) << 8) << 16;
            a1 = getClut(((a1 << 6) & 0x100) + 0x180, (a1 & 3) + ((a1 << 5) | 0xFC)) << 16;
        } else {
            a0 = getTPage(0, x, 0x140, (a1 - 0x80) << 8) << 16;
            a1 = getClut(((s4[7] >> 6) << 4) + 0x140, (s4[7] & 0x3F) + 0x1E0) << 16;
        }
        v1 = a2 << 8;
        a2 = (a2 + t0) << 8;
        v0 = a3 | v1;
        a1 = v0 | a1;
        v0 = a3 + t1;
        v1 = (v0 | v1) | a0;
        a0 = v0 | a2;
        a2 = a3 | a2;
        if (fl) {
            W(0) = s6 | 0x0C000000;
            W(0x18) = v1;
            w = (*(u32 *)((u8 *)node + 8) >> 8) | 0x3E000000;
            W(0xC) = a1;
            H(0x30) = a0;
            H(0x24) = a2;
            W(0x28) = w; W(0x1C) = w; W(0x10) = w; W(4) = w;
        } else {
            W(0) = s6 | 0x09000000;
            if (!(node->flags & 2)) {
                w = 0x2C800000;
            } else {
                w = 0x2E800000;
            }
            w |= 0x8080;
            W(4) = w;
            W(0xC) = a1;
            W(0x14) = v1;
            H(0x24) = a0;
            H(0x1C) = a2;
        }
    }
}
