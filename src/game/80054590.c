#include "game/types.h"

struct Unk54590Obj {
    u8 pad00[0xA0];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    u8 *unkAC;
    u8 padB0[0xB9 - 0xB0];
    u8 unkB9;
    u8 padBA;
    u8 unkBB;
};

struct Unk54590Sub {
    u8 pad00[0x70];
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

extern void func_80053730(void *obj, s32 *out);
extern s32 func_8001A7AC(s32 *vec);
extern void func_8001ABE0(s32 scale, s32 *vec);

#define MUL16(out, a, b) \
    __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

void func_80054590(struct Unk54590Obj *obj, s32 a1, s32 a2, s32 *out, s32 f)
{
    register struct Unk54590Obj *o __asm__("$19");
    register s32 *v __asm__("$17");
    struct Unk54590Sub *sub;
    register s32 r0 __asm__("$7");
    register s32 r1 __asm__("$4");
    s32 s0v;
    s32 x;
    s32 t2;
    s32 tmp;
    s32 lt;
    register s32 z __asm__("$2");
    s32 n;

    o = obj;
    v = out;

    if (a1 != 0) {
        r0 = o->unkA0;
        r1 = o->unkA8;
        t2 = a1;
        if (a2 == 0) {
            if (f != 0) {
                a1 = a1 * 181;
                {
                    register s32 q __asm__("$3");
                    q = a1;
                    if (a1 < 0) {
                        q = a1 + 0xFF;
                    }
                    a1 = q >> 8;
                }
                t2 = a1;
                lt = (a1 < 1);
                a2 = lt ^ 1;
                if (f == 1) {
                    if (lt) {
                        a2 = r0;
                        r0 = a2 - r1;
                        r1 = r1 + a2;
                    } else {
                        a2 = r0;
                        r0 = a2 + r1;
                        r1 = r1 - a2;
                    }
                } else if (a2 != 0) {
                    a2 = r0;
                    r0 = a2 - r1;
                    r1 = r1 + a2;
                } else {
                    a2 = r0;
                    r0 = a2 + r1;
                    r1 = r1 - a2;
                }
            }
        } else {
            a2 = r0;
            r0 = r1;
            r1 = -a2;
        }
        MUL16(tmp, t2, r0);
        v[0] = tmp;
        MUL16(tmp, a1, r1);
        v[2] = tmp;
    } else {
        __asm__ volatile("sw $0, 0(%0)\n\tsw $0, 8(%0)" : : "r"(v));
    }

    n = ((s8 *)o->unkAC)[0x147];
    if (n != 0) {
        t2 = v[0];
        a2 = n * 5;
        v[0] = t2 * a2 / 128;
        a1 = v[2];
        v[2] = a1 * a2 / 128;
    }
    func_80053730(o, v);

    sub = (struct Unk54590Sub *)(o->unkAC + 0xEC);
    if ((o->unkB9 & 1) != 0 || o->unkBB == 0xB || o->unkBB == 0xC ||
        o->unkBB == 0xE || o->unkBB == 0x10) {
        x = sub->unk70;
        MUL16(x, x, 0x666);
        v[0] -= x;
        x = sub->unk78;
        MUL16(x, x, 0x666);
        z = v[2] - x;
    } else {
        s32 vec[2];
        s32 e;
        t2 = sub->unk7C;
        s0v = sub->unk80;
        a1 = sub->unk84;
        vec[0] = t2;
        vec[1] = a1;
        a2 = func_8001A7AC(vec);
        if (a2 < 0x4CCC) {
            a2 = 0;
        } else {
            a2 = a2 - 0x4CCC;
            MUL16(e, a2, a2);
            MUL16(a2, a2, e);
            a2 = a2 * 40;
        }
        func_8001ABE0(a2, vec);
        r0 = ((s8 *)o->unkAC)[0x147];
        t2 = vec[0];
        a1 = vec[1];
        if (r0 != 0) {
            r0 = 4 - r0;
            t2 = t2 + t2 * r0 / 4;
            a1 = a1 + a1 * r0 / 4;
        }
        s0v = s0v < 0 ? 0 : (s0v > 0x10000 ? 0x10000 : s0v);
        if (s0v > 0xD999) {
            s0v = 0x10000;
        } else {
            s0v = s0v + 0x2666;
        }
        MUL16(a2, 0x9800, s0v);
        MUL16(x, t2, a2);
        MUL16(r1, s0v, a2);
        MUL16(a1, a1, a2);
        v[0] += x;
        v[1] += r1;
        v[2] += a1;
        a2 = -0x6000;
        if (r0 != 0) {
            a2 = r0 * a2;
            a2 = a2 / 32;
        }
        x = sub->unk70;
        MUL16(x, x, a2);
        v[0] += x;
        x = sub->unk74;
        MUL16(x, x, -0x6000);
        v[1] += x;
        x = sub->unk78;
        MUL16(x, x, a2);
        z = v[2] + x;
    }
    v[2] = z;
    __asm__ volatile("lui $v1, 0xFFFF\n\tlw $v0, 4(%0)\n\tori $v1, $v1, 0x6800\n\taddu $v0, $v0, $v1\n\tsw $v0, 4(%0)" : : "r"(v) : "memory");
}
