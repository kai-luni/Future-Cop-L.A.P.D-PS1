#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
} Vec;

struct PolyG3 {
    union {
        u32 word;
        struct {
            u8 b0;
            u8 b1;
            u8 b2;
            u8 b3;
        } byte;
    } color;
    u32 code;
    s16 x0, y0;
    u32 color1;
    s16 x1, y1;
    u32 color2;
    s16 x2, y2;
};

extern u8 *D_8009FA90;
extern u8 * volatile D_8009F9C0;

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_8001484C(Vec *v, u32 color0, u32 color1, s32 count, s32 scale, s32 offset_z, s32 angle)
{
    register s32 vx __asm__("$23");
    s32 vy;
    register u32 col0 __asm__("$5");
    register u32 col1 __asm__("$6");
    s32 dz;
    register s32 scl __asm__("$9");
    s32 fp;
    s32 diff;
    register s32 sh __asm__("$3");
    register u32 cw __asm__("$3");
    s32 depth;
    s32 scaled[1];
    struct PolyG3 *poly;
    struct PolyG3 *p1[1];
    struct PolyG3 *p2[1];
    struct PolyG3 *p3;
    s32 sin0[1];
    s32 cos0[1];
    s32 sin1, cos1;
    s32 tag;
    s32 angle_hi[1];
    s32 step;
    s32 i;

    col0 = color0;
    col1 = color1;
    fp = count;
    dz = offset_z;
    scl = scale;

    depth = v->z;
    if (depth - dz <= 0) {
        return;
    }
    sh = scl << 9;
    __asm__("" : "+r"(sh));
    scaled[0] = sh / depth;

    vx = v->x;
    vy = v->y;

    if (D_8009FA90 < D_8009F9C0) goto page_alloc;
    __asm__("");
    poly = (struct PolyG3 *)D_8009FA90;
    goto page_ready;
page_alloc:
    poly = (struct PolyG3 *)D_8009FA90;
    D_8009FA90 = (u8 *)poly + 0x1C;
page_ready:
    cw = 0xE10006A0;
    poly->code = cw;
    poly->color.byte.b3 = 1;
    poly->color.word = (poly->color.word & 0xFF000000) | ((depth - dz) & 0xFFFFFF);

    i = (scaled[0] / 4) * 4;
    if (i < 4) {
        i = 4;
    }
    diff = i - fp;
    fp += (diff >> 31) & diff;
    i = fp - 1;

    col0 = col0 >> 8;
    col0 |= 0x32000000;
    col1 = col1 >> 8;
    tag = (depth - dz) | 0x06000000;

    do {
        if (D_8009FA90 < D_8009F9C0) goto poly_alloc;
        __asm__("");
        poly = (struct PolyG3 *)D_8009FA90;
        goto poly_ready;
    poly_alloc:
        poly = (struct PolyG3 *)D_8009FA90;
        D_8009FA90 = (u8 *)poly + 0x1C;
    poly_ready:
        i--;
        poly->color.word = tag;
        poly->x0 = vx;
        poly->y0 = vy;
        poly->code = col0;
        poly->color2 = col1;
        poly->color1 = col1;
    } while (i >= 0);

    poly -= fp - 1;
    p1[0] = poly + fp / 4;
    p2[0] = poly + fp / 2;
    p3 = poly + (fp * 3) / 4;

    angle_hi[0] = angle >> 8;
    angle &= 0xFF;
    func_8001846C(angle_hi[0], sin0, cos0);

    cos0[0] = (cos0[0] * scaled[0]) >> 16;
    sin0[0] = (sin0[0] * scaled[0]) >> 16;
    step = 0x20000 / fp;

    for (i = 1; i <= fp / 4; i++) {
        func_8001846C(angle_hi[0] + (((i + angle) * step) / 32), &sin1, &cos1);

        poly->x1 = *(u16 *)cos0 + vx;
        poly->y1 = *(u16 *)sin0 + vy;
        p1[0]->x1 = vx - *(u16 *)sin0;
        p1[0]->y1 = *(u16 *)cos0 + vy;
        p2[0]->x1 = vx - *(u16 *)cos0;
        p2[0]->y1 = vy - *(u16 *)sin0;
        p3->x1 = *(u16 *)sin0 + vx;
        p3->y1 = vy - *(u16 *)cos0;

        cos1 = (cos1 * scaled[0]) >> 16;
        sin1 = (sin1 * scaled[0]) >> 16;

        cos0[0] = cos1;
        sin0[0] = sin1;

        poly->x2 = *(u16 *)&cos1 + vx;
        poly->y2 = *(u16 *)&sin1 + vy;
        poly++;

        p1[0]->x2 = vx - *(u16 *)&sin1;
        p1[0]->y2 = *(u16 *)&cos1 + vy;
        p1[0]++;

        p2[0]->x2 = vx - *(u16 *)&cos1;
        p2[0]->y2 = vy - *(u16 *)&sin1;
        p3->x2 = *(u16 *)&sin1 + vx;
        p3->y2 = vy - *(u16 *)&cos1;
        p2[0]++;
        p3++;
    }
}
