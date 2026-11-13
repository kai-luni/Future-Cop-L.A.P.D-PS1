#include "game/types.h"

#define B(o,n) (*(u8 *)((u8 *)(o) + (n)))
#define H(o,n) (*(s16 *)((u8 *)(o) + (n)))
#define U(o,n) (*(u16 *)((u8 *)(o) + (n)))
#define W(o,n) (*(s32 *)((u8 *)(o) + (n)))
#define F(o,n) (*(u32 *)((u8 *)(o) + (n)))

extern void func_8004F764(u8 *, u8 *, s32);
extern void func_8004D958(u8 *);
extern void func_8001846C(s32, s32 *, s32 *);
extern void func_800188D0(s32, s32, s32, u8 *);
extern void func_80010750(u8 *);
extern void func_80010234(u8 *, u8 *, s32);
extern void func_80037EB4(u8 *);
extern s32 func_80095C30(s32, s32);
extern void func_8004FFD0(u8 *, s32, u8 *);

typedef struct { s32 x, y, z; } V3;

#define MULU16(out, a, b) \
    __asm__("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"((s32)(a)), "r"((s32)(b)) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"((s32)(a)), "r"((s32)(b)) : "$8", "$9")

s32 func_8005027C(u8 *s, s32 step, u8 *op)
{
    register u8 *o __asm__("$19");
    register s32 a2 __asm__("$4");
    s32 rem;
    s32 ret;
    s32 v, a, t, d;
    s32 ratio, d0, v1, v2, m1;
    s32 m;
    s32 sv1;
    s32 sv2;
    s32 sv3;
    s16 lim;
    s32 acc;
    u32 fl;
    u32 f4;
    u32 f3;
    u8 *e;
    u8 *map;
    s32 k;
    s32 lv;
    s32 xw;
    s32 r1;
    s32 xk;
    s32 xv;

    d0 = (s16)U(s, 0x40);
    o = op;
    if (d0) {
        v1 = ((d0 >> 31) + d0) ^ (d0 >> 31);
        v1 -= step;
        if (v1 < 0) {
            if (F(s, 0x3C) & 0x10000) {
                MUL16(m1, H(s, 0x30), H(s, 0x34) << 4);
                H(s, 0x32) = m1;
            } else {
                H(s, 0x32) = U(s, 0x30);
            }
            v1 = 0;
        }
        if (H(s, 0x40) < 0) {
            H(s, 0x40) = -v1;
        } else {
            H(s, 0x40) = v1;
        }
    }
    v2 = W(o, 0x84);
    a2 = H(s, 0x32);
    if (v2 < a2 && H(s, 0x40) == 0) {
        v2 += (step * H(s, 0x36)) / 16;
        if (a2 < v2) {
            v2 = a2;
        }
    }
    if (a2 < v2) {
        v2 -= (step * H(s, 0x38)) / 16;
        if (v2 < a2) {
            v2 = a2;
        }
        if (v2 < 0) {
            v2 = 0;
        }
    }
    rem = v2 * step;
    W(o, 0x84) = v2;
    if (rem <= 0) {
        return 0;
    }
    ret = 0;
loop:
    t = W(s, 0x1C) + rem;
    W(s, 0x1C) = t;
    k = t >> 8;
    lv = U(s, 0x20) << 16;
    k = k < (lv >> 16);
    if (!k) {
        if (H(s, 0x46) < 0) {
            func_8004F764(s, o, 0);
            {
            s32 h46 = H(s, 0x46);
            if (h46 < 0) {
                if (h46 == -2 && (F(s, 0x3C) & 0x8000)) {
                    func_8004D958(o);
                } else {
                    F(s, 0x3C) |= 0x40;
                    W(o, 0x84) = 0;
                    W(s, 0x1C) = H(s, 0x20) << 8;
                }
                return 0;
            }
            }
            ret |= 2;
        } else {
            u32 f1 = F(s, 0x3C);
            if (f1 & 0x800) {
                s32 x = H(s, 0x54);
                s32 y = H(s, 0x56);
                u16 c = U(s, 0x56);
                if (x != y && !(f1 & 0x100)) {
                    U(s, 0x52) = c;
                    U(s, 0x54) = c;
                    func_8001846C(y, &sv3, (s32 *)(s + 0x68));
                    ret |= 2;
                }
                {
                    s32 x2 = H(s, 0x4E);
                    s32 y2 = H(s, 0x50);
                    u16 c2 = U(s, 0x50);
                    if (x2 != y2 && !(F(s, 0x3C) & 0x1000)) {
                        U(s, 0x4C) = c2;
                        U(s, 0x4E) = c2;
                        func_8001846C(y2, (s32 *)(s + 0x64), (s32 *)(s + 0x6C));
                        ret |= 2;
                    }
                }
            }
            {
                u32 f2 = F(s, 0x3C);
                if (((f2 >> 28) & 3) == 1) {
                    k = 0xCFFFFFFF;
                    k &= f2;
                    k |= 0x20000000;
                    F(s, 0x3C) = k;
                }
            }
        }
        U(s, 0x04) = U(s, 0x04) + U(s, 0x0A);
        U(s, 0x06) = U(s, 0x06) + U(s, 0x0C);
        U(s, 0x08) = U(s, 0x08) + U(s, 0x0E);
        *(V3 *)(o + 0x78) = *(V3 *)(s + 0x70);
        U(s, 0x0A) = U(s, 0x10);
        U(s, 0x0C) = U(s, 0x12);
        U(s, 0x0E) = U(s, 0x14);
        if (H(s, 0x48) == H(s, 0x46) && (F(s, 0x3C) & 0x400)) {
            func_800188D0((H(s, 0x50) + 0x800) & 0xFFF, 0, 0, o + 0x2C);
            func_80010750(o + 0x2C);
            func_80010234((u8 *)W((u8 *)W(o, 0x28), 0x34) + ((H(o, 0x70) << 4) + 8), o + 0x5C, 1);
            func_80037EB4(o);
            if (!(F(s, 0x3C) & 0x1000)) {
                v = func_80095C30(W(o, 0x78), W(o, 0x80));
                U(s, 0x50) = v & 0xFFF;
                U(s, 0x4C) = v & 0xFFF;
                U(s, 0x4E) = v & 0xFFF;
            }
            ret |= 3;
        }
        k = H(s, 0x44);
        if (k >= 0) {
            map = (u8 *)W(s, 0);
            if (k < H(map, 2)) {
                k = k * 12;
                lv = B((u8 *)(k + (s32)map), 8) & 0xF;
                if (lv > 0) {
                    u8 *e1 = map + (k + 4);
                    B(e1, 4) = (B(e1, 4) & 0xF0) | (lv - 1);
                }
            }
        }
        xk = H(s, 0x46);
        if (xk >= 0) {
            map = (u8 *)W(s, 0);
            if (xk < H(map, 2)) {
                xk = xk * 12;
                lv = B((u8 *)(xk + (s32)map), 8) & 0xF;
                if (lv < 0xF) {
                    u8 *e2 = map + (xk + 4);
                    B(e2, 4) = (B(e2, 4) & 0xF0) | (lv + 1);
                }
            }
        }
        {
            u16 i3;
            s32 sp20;
            u16 n48, n44, n22;
            i3 = U(s, 0x46);
            rem = W(s, 0x1C) - (H(s, 0x20) << 8);
            H(s, 0x46) = -1;
            n48 = U(s, 0x48);
            n44 = U(s, 0x44);
            n22 = U(s, 0x22);
            W(s, 0x1C) = 0;
            U(s, 0x4A) = n48;
            U(s, 0x48) = n44;
            U(s, 0x44) = i3;
            U(s, 0x20) = n22;
        }
    } else {
        ret |= 1;
        MULU16(t, t, 0x80000000U / (u32)(lv >> 17));
        {
            s32 mx;
            MUL16(mx, H(s, 0x0A), t);
            W(o, 0x50) = (H(s, 0x04) << 8) + mx;
        }
        MUL16(m, H(s, 0x0C), t);
        v = (H(s, 0x06) << 8) + m;
        if (!(F(s, 0x3C) & 0x10)) {
            v += H(s, 0x26) << 4;
        }
        W(o, 0x54) = v;
        {
            s32 mz;
            MUL16(mz, H(s, 0x0E), t);
            W(o, 0x58) = (H(s, 0x08) << 8) + mz;
        }
        fl = F(s, 0x3C);
        t = (fl >> 28) & 3;
        if (t == 2) {
            t = W(s, 0x1C);
            if ((t >> 8) >= H(s, 0x24)) {
                if (!(fl & 0x800)) {
                    if (!(fl & 0x1000)) {
                        u16 c1 = U(s, 0x50);
                        U(s, 0x4C) = c1;
                        U(s, 0x4E) = c1;
                    }
                    if (!(F(s, 0x3C) & 0x100)) {
                        u16 c2 = U(s, 0x56);
                        U(s, 0x52) = c2;
                        U(s, 0x54) = c2;
                    }
                }
                F(s, 0x3C) &= 0xCFFFFFFF;
                if (F(s, 0x3C) & 0x20) {
                    H(s, 0x5A) = 0;
                    H(s, 0x58) = 0;
                }
            } else {
                ret |= 3;
                if (!(fl & 0x800)) {
                    func_8004FFD0(s, t, o);
                    goto l900;
                }
                goto done;
            }
l900:
            f3 = F(s, 0x3C);
            if (f3 & 0x800) {
                goto done;
            }
            if (!(f3 & 0x100)) {
                func_8001846C(H(s, 0x54), &sv1, (s32 *)(s + 0x68));
            }
            if (F(s, 0x3C) & 0x1000) {
                goto done;
            }
            func_8001846C(H(s, 0x4E), (s32 *)(s + 0x64), (s32 *)(s + 0x6C));
            goto done;
        }
        r1 = (H(s, 0x20) << 8) - W(s, 0x1C);
        if (t == 1 && (r1 >> 8) < H(s, 0x24)) {
            if (fl & 0x800) {
                goto done;
            }
            func_8004FFD0(s, -r1, o);
            ret |= 3;
            if (F(s, 0x3C) & 0x1000) {
                goto done;
            }
            func_8001846C(H(s, 0x54), &sv2, (s32 *)(s + 0x68));
            func_8001846C(H(s, 0x4E), (s32 *)(s + 0x64), (s32 *)(s + 0x6C));
            goto done;
        }
        if (H(s, 0x46) < 0) {
            func_8004F764(s, o, 1);
        }
        goto done;
    }
    if (rem > 0 && !(F(s, 0x3C) & 0x40)) {
        goto loop;
    }
done:
    f4 = F(s, 0x3C);
    if (!(f4 & 0x10000)) {
        if (((f4 >> 28) & 3) != 0) {
            s32 dd = (H(s, 0x50) - H(s, 0x4C)) & 0xFFF;
            if (dd != 0) {
                if (f4 & 2) {
                    H(s, 0x32) = (s16)U(s, 0x30) >> 2;
                } else {
                    s32 w;
                    if (dd >= 0x7ED) {
                        dd -= 0x1000;
                    }
                    dd = ((dd >> 31) + dd) ^ (dd >> 31);
                    if (dd > 0x400) {
                        dd = 0x400;
                    }
                    w = 0x1000 - H(s, 0x34);
                    if (w > 0) {
                        s32 r;
                        w = 0x10000 - ((w * dd) >> 6);
                        MUL16(r, H(s, 0x30), w);
                        H(s, 0x32) = r;
                    }
                }
                F(s, 0x3C) |= 0x10000;
            }
        }
    } else if (((f4 >> 28) & 3) == 0) {
        U(s, 0x32) = U(s, 0x30);
        F(s, 0x3C) = f4 & 0xFFFEFFFF;
    }
    a = U(s, 0x30);
    if (H(s, 0x32) < ((s16)a >> 3) && H(s, 0x40) == 0) {
        U(s, 0x32) = a;
    }
    return ret;
}
