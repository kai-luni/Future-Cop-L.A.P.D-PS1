#include "game/types.h"

typedef struct {
    s32 w0;
    s16 s4;
    s16 pad6;
    s32 w8;
    u8 pad0C[4];
    s32 w10;
    u8 pad14[4];
    s32 w18;
    u8 pad1C[4];
} Vtx17;

typedef struct {
    u32 w[13];
} Prim17;

extern Prim17 *D_8009F9F0;
extern Prim17 *D_8009FA28;
extern u16 D_8009F580;

extern void func_80015F14(void *a0, void *a1, void *a2);

s32 func_800172A8(s32 arg0, u32 arg1, u32 *a2, Vtx17 *arg3)
{
    Prim17 *prim;
    u32 tmp;
    register s32 t0 __asm__("$8");
    register s32 t1 __asm__("$9");
    register s32 t2 __asm__("$10");
    u32 a4;
    s32 q;
    register u32 fl __asm__("$8");
    register u32 hi __asm__("$3");
    register u32 tv __asm__("$2");
    register void *r4 __asm__("$4");
    register u32 r5 __asm__("$5");
    register void *r6 __asm__("$6");
    u32 v;
    u32 idx;
    u32 lb;
    register s32 w18 __asm__("$3");
    register u32 lo __asm__("$3");
    register u32 hs __asm__("$2");
    register u32 msk __asm__("$2");
    register u32 c1 __asm__("$5");
    u32 c2;
    register u32 base __asm__("$17");
    void *vp;

    fl = arg1;
    base = D_8009F580;
    vp = (u8 *)arg3 + 0x20;

    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    prim = D_8009F9F0;
    goto L_done;
L_alloc:
    prim = D_8009F9F0;
    D_8009F9F0 = (Prim17 *)((u8 *)prim + 0x34);
L_done:
    tmp = (fl & 2) << 24;
    lb = fl & 0xFF;
    prim->w[0] = (prim->w[0] & 0xFF000000) | (arg3->s4 & 0xFFFFFF);
    idx = lb >> 4;
    r5 = a2[0];
    tmp = tmp | (r5 >> 8);
    t0 = ((s32 *)arg3)[0];
    t1 = ((s32 *)arg3)[2];
    t2 = ((s32 *)arg3)[4];
    switch (idx) {
    case 0:
    case 2:
        r4 = vp;
        r5 = (u32)&tmp;
        r6 = &prim->w[1];
        tv = 4;
        prim->w[2] = t0;
        prim->w[3] = t1;
        prim->w[4] = t2;
        __asm__("" ::: "memory");
        ((u8 *)prim)[3] = tv;
        __asm__("" ::: "memory");
        tv = tmp;
        hi = 0x20000000;
        goto L_call;
    case 1:
    case 3:
        prim->w[2] = t0;
        prim->w[4] = t1;
        prim->w[6] = t2;
        ((u8 *)prim)[3] = 7;
        tmp |= 0x24000000;
        func_80015F14(vp, &tmp, &prim->w[1]);
        c1 = a2[1];
        c2 = a2[3];
        msk = 0xFFFF0000;
        lo = ((c1 & 0xFFFF) + base) | (c2 & msk);
        prim->w[3] = lo;
        hs = (c1 >> 16) + base;
        prim->w[5] = hs | (c2 << 16);
        v = *(u16 *)&a2[2] + base;
        goto L_sh1C;
    case 4:
    case 6:
        r4 = vp;
        r5 = (u32)&tmp;
        r6 = &prim->w[1];
        __asm__ volatile ("" : : "r" (r6));
        prim->w[2] = t0;
        prim->w[3] = t1;
        prim->w[5] = t2;
        w18 = ((s32 *)arg3)[6];
        ((u8 *)prim)[3] = 5;
        __asm__ volatile ("sw %2, 16(%1)\n\tlw %0, %3" : "=r" (tv) : "r" (prim), "r" (w18), "m" (tmp) : "memory");
        hi = 0x28000000;
    L_call:
        tmp = tv | hi;
        func_80015F14(r4, (void *)r5, r6);
        return 1;
    case 5:
    case 7:
        prim->w[2] = t0;
        prim->w[4] = t1;
        prim->w[8] = t2;
        prim->w[6] = ((s32 *)arg3)[6];
        ((u8 *)prim)[3] = 9;
        tmp |= 0x2C000000;
        func_80015F14(vp, &tmp, &prim->w[1]);
        c1 = a2[1];
        c2 = a2[3];
        msk = 0xFFFF0000;
        lo = ((c1 & 0xFFFF) + base) | (c2 & msk);
        prim->w[3] = lo;
        hs = (c1 >> 16) + base;
        prim->w[5] = hs | (c2 << 16);
        r5 = a2[2];
        v = r5 + base;
        *(s16 *)((u8 *)prim + 0x24) = v;
        __asm__("" ::: "memory");
        v = r5 >> 16;
        v = v + base;
    L_sh1C:
        *(s16 *)((u8 *)prim + 0x1C) = v;
        return 1;
    }
    return 1;
}
