#include "game/types.h"

struct S {
    u8 p0[0x8F4];
    volatile s32 s8F4;
    volatile s32 s8F8;
    volatile s32 s8FC;
    volatile s32 s900;
    volatile s32 s904;
    volatile s32 s908;
    volatile s32 s90C;
    s32 s910;
    volatile u32 d[32];
    volatile s32 s994;
    volatile s32 s998;
    volatile s32 s99C;
    volatile s32 s9A0;
    volatile s32 s9A4;
    volatile s32 s9A8;
    u8 p9AC[4];
    u8 s9B0[0x1BB0 - 0x9B0];
    volatile s32 s1BB0;
};

extern struct S *D_8009AB08;

extern void func_8008D640(s32);
extern void func_8008D650(s32);
extern void func_8008D670(s32, s32, volatile u32 *);
extern void func_8008D680(s32, s32, volatile u32 *);
extern void *func_8001E318(volatile void *, void *, u32);

s32 func_8001FAB8(void)
{
    register struct S *p asm("$3");
    register s32 a asm("$4");
    u32 i;
    s32 sum;
    volatile u32 *e;
    s32 x, y;
    struct S *q;

    p = D_8009AB08;
    p->s1BB0 = 0;
    p->s9A8 = 1;
    a = p->s8F4;
    if (a == 1) {
        p->s904 = a;
        if (p->s8F8 != 0) {
            p->s900 = -3;
            p->s8F8 = 0;
            p->s8F4 = 0;
            p->s904 = 0;
            func_8008D640(p->s8FC);
            return 0;
        }
        e = p->d;
        p->s998 = 0;
        p->s99C = 0x10;
        p->s90C = 0;
        p->s994 = 0;
        p->s9A4 = 0;
        p->s9A0 = 3;
        p->s8F4 = 2;
        p->s8F8 = a;
        x = p->s8FC;
        y = p->s998;
        p->s910 = (s32)p->s9B0;
        func_8008D680(x, y, e);
        return 0;
    }
    sum = 0;
    if (p->s908 != 0 && p->s904 == 0) {
        p->s908 = 1;
        p->s8F4 = 1;
        func_8008D650(p->s8FC);
        return 0;
    }
    if (D_8009AB08->s8F8 == 5) {
        D_8009AB08->s904 = 1;
        D_8009AB08->s8F8 = 6;
    }
    q = D_8009AB08;
    if (q->s8F8 == 2) {
        q->s9A0 = 3;
        q->s8F4 = 2;
        q->s8F8 = 0;
        q->s900 = -4;
        func_8008D680(q->s8FC, q->s998, q->d);
        return 0;
    }
    if (q->s8F8 == 3 || q->s8F8 == 6) {
        q->s9A0 = 3;
        q->s8F4 = q->s8F8;
        q->s8F8 = 0;
        if (q->s90C != 0) {
            func_8001E318(&q->d[2], (void *)q->s910, 0x78);
            q = D_8009AB08;
            for (i = 2; i < 32; i++) {
                sum += (q->d[i] ^ q->d[32 - i]) + i;
            }
            D_8009AB08->d[0] = D_8009AB08->s998;
            D_8009AB08->d[1] = sum;
        } else {
            func_8001E318(q->d, (void *)q->s910, 0x80);
        }
        q = D_8009AB08;
        q->s900 = -5;
        func_8008D670(q->s8FC, q->s998, q->d);
        return 0;
    }
    q->s8F4 = 0;
    func_8008D640(q->s8FC);
    return 0;
}
