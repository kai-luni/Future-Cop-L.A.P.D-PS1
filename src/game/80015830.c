#include "game/types.h"
typedef struct { s16 x,y,z,pad; } Point;
typedef struct { u8 pad[4]; u8 a,b,c,d; } Input;
typedef struct { s16 xy[8]; s32 z; } Output;
extern Point *D_8009F578;
extern u8 *D_8009F57C;
s32 func_80015830(Input *in, Output *out, s16 *width)
{
    register Point *p __asm__("$9");
    register Point *q __asm__("$10");
    register Output *result __asm__("$11") = out;
    s32 w1;
    register s32 w2 __asm__("$12");
    register s32 dx __asm__("$4"), dy __asm__("$6");
    register s32 a __asm__("$8"), b __asm__("$5");
    s32 t,ic,pz;
    register s32 qz __asm__("$5");
    register s32 x1 __asm__("$3"), y1 __asm__("$5"), x2 __asm__("$6"), y2 __asm__("$4");
    register s32 scale1 __asm__("$5"), scale2 __asm__("$7");
    register s32 dist __asm__("$3");
    register s32 num __asm__("$2");
    register s32 widthfirst __asm__("$5");
    {
      register s32 off __asm__("$3");
      s32 ia = in->a;
      register s32 ib __asm__("$5") = in->b;
      u8 *flags = D_8009F57C;
      s32 fa = flags[ia], fb = flags[ib];
      __asm__ volatile ("" : "=r" (fa), "=r" (fb), "=r" (result), "=r" (ib) : "0" (fa), "1" (fb), "2" (result), "3" (ib));
      p = D_8009F578;
      __asm__ volatile ("" : "=r" (p) : "0" (p));
      if ((fa | fb) & 1) return 0;
      num = ib << 3;
      __asm__ volatile ("" : "=r" (num) : "0" (num));
      q = (Point *)((u8 *)p + num);
      off = ia << 3;
      __asm__ volatile ("" : "=r" (off) : "0" (off));
      ic = in->c;
      __asm__ volatile ("" : "=r" (q), "=r" (ic) : "0" (q), "1" (ic));
      p = (Point *)((u8 *)p + off);
    }
    pz = p->z;
    widthfirst = width[ic + 2];
    __asm__ volatile ("" : "=r" (widthfirst) : "0" (widthfirst));
    num = widthfirst << 9;
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    w1 = num / pz;
    __asm__ volatile ("" : "=r" (w1) : "0" (w1));
    {
    register s32 widthsecond __asm__("$7"), sum __asm__("$3");
    register s16 *wp __asm__("$2");
    qz = q->z;
    wp = (s16 *)((in->d << 1) + (u32)width);
    sum = pz + qz;
    __asm__ volatile ("" : "=r" (wp), "=r" (sum) : "0" (wp), "1" (sum));
    widthsecond = wp[2];
    __asm__ volatile ("" : "=r" (widthsecond), "=r" (qz) : "0" (widthsecond), "1" (qz));
    result->z = sum / 2;
    {
        register s32 qy __asm__("$6") = q->y;
        register s32 py __asm__("$3") = p->y;
        register s32 px __asm__("$4") = p->x;
        register s32 qx __asm__("$2") = q->x;
        __asm__ volatile ("" : "=r" (qy), "=r" (py), "=r" (px), "=r" (qx) : "0" (qy), "1" (py), "2" (px), "3" (qx));
        dy = qy - py;
        dx = px - qx;
    }
    num = widthsecond << 9;
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    w2 = num / qz;
    }
    if (dy < 0) {
        __asm__ volatile ("" : "=r" (dy) : "0" (dy));
        a = -dy;
    } else a = dy;
    if (dx < 0) {
        __asm__ volatile ("" : "=r" (dx) : "0" (dx));
        b = -dx;
    } else b = dx;
    if (a < b) { a ^= b; b ^= a; a ^= b; }
    if ((a >> 1) < b) {
        t = a + b;
        t = (t << 6) + (t << 3) + (t << 1) + a + (b << 5);
        b = a - b;
        a = t >> 7;
    }
    num = a + (b >> 2);
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    dist = num - (b >> 6);
    if (dist > 0) {
        num = w1 << 10;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        scale1 = num / dist;
        num = w2 << 10;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        scale2 = num / dist;
        x1 = dy * scale1;
        __asm__ volatile ("" : "=r" (x1) : "0" (x1));
        num = dx * scale1;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        x2 = dy * scale2;
        __asm__ volatile ("" : "=r" (x2) : "0" (x2));
        y2 = dx * scale2;
        x1 >>= 10;
        y1 = num >> 10;
        x2 >>= 10;
        y2 >>= 10;
    } else { x1 = x2 = y1 = y2 = 0; }
    result->xy[0] = p->x + x1;
    result->xy[1] = p->y + y1;
    result->xy[2] = p->x - x1;
    result->xy[3] = p->y - y1;
    result->xy[4] = q->x + x2;
    result->xy[5] = q->y + y2;
    result->xy[6] = q->x - x2;
    result->xy[7] = q->y - y2;
    return 1;
}
