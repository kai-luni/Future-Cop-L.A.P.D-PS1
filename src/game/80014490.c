#include "game/types.h"

struct Src {
    s16 x;
    s16 y;
    s16 len;
};

struct Poly {
    union {
        u32 word;
        struct {
            u8 b0;
            u8 b1;
            u8 b2;
            u8 b3;
        } byte;
    } color;
    u32 f04;
    s16 x0;
    s16 y0;
    u32 f0C;
    s16 x1;
    s16 y1;
    u32 f14;
    s16 x2;
    s16 y2;
};

extern u8 *D_8009FA90;
extern u8 * volatile D_8009F9C0;
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_80014490(struct Src *src, u32 p1, u32 p2, s32 p3, s32 p4, s32 p5, s32 p6)
{
    s32 sine;
    s32 cosine;
    struct Poly *poly;
    struct Poly *q0;
    struct Poly *q1;
    struct Poly *q2;
    struct Poly *q3;
    u8 *cursor;
    register u8 *current __asm__("$3");
    register s32 len __asm__("$8");
    register s32 guard __asm__("$2");
    register s32 dividend __asm__("$3");
    register s32 inner __asm__("$9");
    s32 fp;
    s32 quot;
    s32 angle;
    s32 step;
    s32 shift;
    register s32 argangle __asm__("$13");
    register s32 sx __asm__("$23");
    register s32 sy __asm__("$22");
    register s32 delta __asm__("$3");
    register s32 clipped __asm__("$2");
    register s32 cosprod __asm__("$3");
    register s32 sinprod __asm__("$2");
    register s32 va __asm__("$4");
    register s32 vb __asm__("$3");
    s32 accu;
    s32 i;
    register u32 header __asm__("$3");
    u32 c0;
    u32 c1;
    register u32 c2 __asm__("$6");
    register u16 x0_3 __asm__("$5");
    register u16 y0_3 __asm__("$6");

    quot = p4;
    __asm__("" : "=r" (quot) : "0" (quot));
    inner = p5;
    len = src->len;
    argangle = p6;
    fp = p3;
    guard = len - inner;
    if (guard <= 0) {
        return;
    }
    __asm__("" : "=r" (inner) : "0" (inner));
    dividend = quot << 9;
    quot = dividend / len;
    sx = src->x;
    sy = src->y;

    cursor = D_8009FA90;
    if (cursor < D_8009F9C0) {
        D_8009FA90 = cursor + 0x1C;
    }
    poly = (struct Poly *)cursor;
    header = 0xE10006A0;
    poly->f04 = header;
    poly->color.byte.b3 = 1;
    poly->color.word = (poly->color.word & 0xFF000000) | ((len - inner) & 0xFFFFFF);

    i = quot << 2;
    if (i < 4) {
        i = 4;
    }
    delta = i - fp;
    clipped = (delta >> 31) & delta;
    __asm__("" : "=r" (clipped) : "0" (clipped));
    fp = fp + clipped;
    i = fp - 1;

    c2 = p2 >> 8;
    __asm__("" : "=r" (c2) : "0" (c2));
    c2 |= 0x32000000;
    c1 = p1 >> 8;
    c0 = (len - inner) | 0x06000000;

    do {
        current = D_8009FA90;
        __asm__("" : "=r" (current) : "0" (current));
        if (current >= D_8009F9C0) {
            cursor = current;
            __asm__ volatile ("");
            i--;
            goto write_poly;
        }
        cursor = current;
        D_8009FA90 = cursor + 0x1C;
        i--;
write_poly:
        poly = (struct Poly *)cursor;
        poly->color.word = c0;
        poly->f04 = c2;
        poly->f14 = c1;
        poly->f0C = c1;
    } while (i >= 0);

    poly = poly - (fp - 1);
    q1 = poly + fp / 4;
    q2 = poly + fp / 2;
    q3 = poly + (fp * 3) / 4;
    q0 = poly;

    step = 0x20000 / fp;
    accu = step;
    i = 1;
    __asm__("" : "=r" (argangle) : "0" (argangle));
    angle = argangle >> 8;
    shift = argangle & 0xFF;
    while (fp / 4 >= i) {
        func_8001846C(angle + accu / 32, &sine, &cosine);
        sinprod = cosine;
        cosprod = sinprod * quot;
        va = cosprod >> 16;
        cosine = va;
        va = va >> shift;
        sinprod = sine * quot;
        vb = sinprod >> 16;
        sine = vb;
        vb = vb >> shift;
        i++;
        accu = accu + step;
        q0->x0 = cosine + sx;
        q0->y0 = sine + sy;
        q0->x1 = sx - vb;
        q0->y1 = sy + va;
        q0->x2 = sx + vb;
        q0->y2 = sy - va;
        q0++;
        q1->x0 = sx - sine;
        q1->y0 = cosine + sy;
        q1->x1 = sx + va;
        q1->y1 = sy + vb;
        q1->x2 = sx - va;
        q1->y2 = sy - vb;
        q1++;
        q2->x0 = sx - cosine;
        q2->y0 = sy - sine;
        q2->x1 = sx + vb;
        q2->y1 = sy - va;
        q2->x2 = sx - vb;
        q2->y2 = sy + va;
        q2++;
        x0_3 = sine + sx;
        y0_3 = sy - cosine;
        q3->x0 = x0_3;
        q3->y0 = y0_3;
        q3->x1 = sx - va;
        q3->y1 = sy - vb;
        q3->x2 = sx + va;
        q3->y2 = sy + vb;
        q3++;
    }
}
