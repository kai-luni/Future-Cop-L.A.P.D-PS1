#include "game/types.h"

#define B(o,n) (*(u8 *)((u8 *)(o) + (n)))
#define H(o,n) (*(s16 *)((u8 *)(o) + (n)))
#define U(o,n) (*(u16 *)((u8 *)(o) + (n)))
#define W(o,n) (*(s32 *)((u8 *)(o) + (n)))
#define F(o,n) (*(u32 *)((u8 *)(o) + (n)))
#define P(o,n) (*(u8 **)((u8 *)(o) + (n)))
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

typedef struct { s32 x, y, z; } Vec;

extern s32 func_80044B34(u8 *, void *, s32, s32);
extern void func_800175F8(void *, s32);
extern void func_8002E528(u8 *);
extern void func_8002E2BC(u8 *, s32);
extern u8 func_80060238(void *, s32, u8 *, s32, s32);
extern void func_800448D8(void *, s32, s32, u8);
extern void func_8005F6E8(void *, u8 *, u8 *, s32);
extern s32 func_80095C30(s32, s32);
extern void func_800188D0(s32, s32, s32, void *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern void func_8001AC28(void *, void *, s32, void *);
extern void func_800457B8(s32, s32);

#define FMULU(a,b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })

s32 func_8002E630(u8 *obj, s32 dt)
{
    s32 s0;
    s32 a2;
    s32 ang;
    u8 *s2;
    u8 *t;
    u16 timer;
    u8 odd;
    register u32 sw __asm__("$3");

    if (B(obj,0xDC) == 4) {
        s0 = func_80044B34(obj, obj + 0xC8, dt, 0);
        func_800175F8(obj + 0x28, H(obj,0xD4));
        s0 = (u32)s0 >> 1;
        if (s0 != 3) {
            goto done;
        }
        H(obj,0x1C) = -1;
        B(obj,0xDC) = 5;
        goto done;
    }
    if (B(obj,0xC)) {
        func_8002E528(obj);
    }
    if ((B(obj,0xDD) & 1) && H(obj,0xE2) > 0) {
        goto after_timer;
    }
    timer = U(obj,0xE2) - dt;
    U(obj,0xE2) = timer;
    if ((s16)timer > 0) {
        goto after_timer;
    }
    switch (B(obj,0xDC)) {
    case 0: {
        Vec save;
        u16 t0;
        save = *(Vec *)(obj + 0x50);
        W(obj,0x50) += W(obj,0x100);
        t0 = U(obj,0xA4);
        s0 = t0 & 7;
        W(obj,0x54) += W(obj,0x104);
        W(obj,0x58) += W(obj,0x108);
        U(obj,0xA4) = t0 & 0xFFF8;
        if (func_80060238(obj + 0xA0, dt, obj, 0, 0)) {
            if ((B(obj,0xDD) & 0x80) && H(obj,0xD4) != H(P(obj,0xCC),6)) {
                goto after_timer;
            }
            func_8002E2BC(obj, 1);
        } else {
            H(obj,0xE2) = 0;
        }
        U(obj,0xA4) |= s0;
        *(Vec *)(obj + 0x50) = save;
        break;
    }
    case 1:
        func_8002E2BC(obj, 2);
        break;
    case 2:
        if (B(obj,0xD9) != B(obj,0x110)) {
            func_8002E2BC(obj, 3);
        }
        break;
    case 3:
        func_8002E2BC(obj, 0);
        break;
    }
after_timer:
    switch (B(obj,0xDC)) {
    case 1:
    case 3:
        goto case_13;
    case 0:
        goto case_0;
    case 2:
        goto case_2;
    default:
        goto tail;
    }
case_2:
    if (W(P(obj,0x28),0x10)) {
        a2 = dt;
        if (B(obj,0xE6)) {
            a2 = (B(obj,0xE6) * dt) / 16;
        }
        s2 = obj + 0xC8;
        s0 = func_80044B34(obj, s2, a2, 0);
        if (W(obj,0xA0)) {
            s0 = (u32)s0 >> 1;
            if (s0) {
                sw = s0 & 0x3F;
                switch (sw) {
                case 2:
                    func_8005F6E8(obj + 0xA0, obj, obj, 1);
                    break;
                case 1:
                    if (H(obj,0xE2) > 0 && B(obj,0xD9) != B(obj,0x110)) {
                        if (func_80060238(obj + 0xA0, dt, obj, 0, 0)) {
                            func_800448D8(s2, B(obj,0x110), 1, 1);
                        }
                    }
                    break;
                }
            }
        }
    } else {
        func_80060238(obj + 0xA0, dt, obj, 0, 1);
    }
    if ((B(obj,0xDD) & 0x10) || ((B(obj,0xDD) & 4) && P(obj,0xB8))) {
        if (B(obj,0xDD) & 0x10) {
            ang = H(obj,0xE4) + 0x800;
        } else {
            s32 vec[3];
            vec[0] = W(P(obj,0xB8),0x5C) - W(obj,0x5C);
            vec[1] = W(P(obj,0xB8),0x60) - W(obj,0x60);
            vec[2] = W(P(obj,0xB8),0x64) - W(obj,0x64);
            ang = func_80095C30(vec[0], vec[2]);
        }
    {
        s32 cur;
        s32 base;
        s32 d;
        s32 step;
        s32 ad;

        ang &= 0xFFF;
        cur = H(obj,0xE4);
        base = U(obj,0xE4);
        if (cur == ang) {
            goto tail;
        }
        d = (ang - cur) & 0xFFF;
        if (d > 0x800) {
            d -= 0x1000;
        }
        step = dt << 3;
        ad = ABS(d);
        if (ad < step) {
            step = ad;
        }
        if (d < 0) {
            step = -step;
        }
        base += step;
        base &= 0xFFF;
        U(obj,0xE4) = base;
        func_800188D0(base, 0, 0, obj + 0x2C);
        func_80010750(obj + 0x2C);
        func_80010234(P(P(obj,0x28),0x34) + ((H(obj,0x70) << 4) + 8), obj + 0x5C, 1);
        *(Vec *)(obj + 0x78) = *(Vec *)(obj + 0x44);
    }
    }
    goto tail;
case_0:
    if (W(P(obj,0x28),0x10)) {
        a2 = dt;
        if (B(obj,0xE6)) {
            a2 = (B(obj,0xE6) * dt) / 16;
        }
        func_80044B34(obj, obj + 0xC8, a2, 0);
        goto tail;
    }
    goto tail2;
case_13:
    if (W(obj,0xE8) > 0) {
        s0 = W(obj,0xE8) * dt;
        if (B(obj,0xDC) == 3) {
            s0 = -s0;
            if (W(obj,0xEC) + s0 < 0) {
                s0 = -W(obj,0xEC);
                H(obj,0xE2) = 0;
            }
        } else {
            if (W(obj,0xF0) < W(obj,0xEC) + s0) {
                s0 = W(obj,0xF0) - W(obj,0xEC);
                H(obj,0xE2) = 0;
            }
        }
        a2 = W(obj,0xEC) + s0;
        W(obj,0xEC) = a2;
        if (W(obj,0xF0) > 0) {
            a2 = FMULU(a2, (u32)0x80000000 / (u32)(W(obj,0xF0) >> 1));
        } else {
            a2 = 0x10000;
        }
        func_8001AC28(obj + 0xF4, obj + 0x100, a2, obj + 0x50);
        if (W(P(obj,0x28),0x10)) {
            func_80044B34(obj, obj + 0xC8, ABS(s0), 0);
        }
    } else {
        odd = func_80044B34(obj, obj + 0xC8, dt, 0) & 1;
        if (B(obj,0xDC) == 1 || B(obj,0xDC) == 3) {
            if (odd) {
                H(obj,0xE2) = 0;
            }
        }
    }
tail:
    if (W(P(obj,0x28),0x10)) {
        func_800175F8(obj + 0x28, H(obj,0xD4));
    }
tail2:
    func_80010750(obj + 0x2C);
    func_80010234(P(P(obj,0x28),0x34) + ((H(obj,0x70) << 4) + 8), obj + 0x5C, 1);
    if (B(obj,0xDD) & 0x40) {
        if (B(obj,0xDC) == 1 || B(obj,0xDC) == 3) {
            B(obj,0x21) = B(obj,0x113);
        } else if (B(obj,0x113)) {
            if (!(F(obj,0x24) & 0x400000)) {
                goto done;
            }
            func_800457B8(W(obj,8), (s8)B(obj,0x22));
            B(obj,0x21) = 0;
            F(obj,0x24) &= ~0x400000;
        }
    }
done:
    return 0;
}
