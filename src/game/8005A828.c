#include "game/types.h"

struct LineF3 {
    u32 tag;
    u32 code;
    s16 xy[6];
};

extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;
extern s16 D_8009AFB4[];

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_8005A828(s32 origin, u32 angle, s32 code)
{
    struct LineF3 *line;
    s32 values[2];
#define sine values[0]
#define cosine values[1]
    register s32 s __asm__("$6");
    register s32 c __asm__("$7");
    s32 i;
    register s32 bias __asm__("$9");
    s16 *out;
    register s16 *points __asm__("$4");
    register s32 ctr __asm__("$8");
    register s32 *arg1 __asm__("$5");
    register s32 *arg2 __asm__("$6");
    register u32 arg0 __asm__("$4");
    s32 saved_code;

    saved_code = code;
    arg0 = angle;
    arg1 = &sine;
    __asm__ volatile ("" : "+r"(arg0), "+r"(arg1), "+r"(saved_code));
    arg2 = &cosine;
    func_8001846C(arg0, arg1, arg2);
    line = (struct LineF3 *)D_8009FA90;
    if (D_8009FA90 < D_8009F9C0) {
        D_8009FA90 += 0x1C;
    }
    ctr = 0;
    bias = 0x2C0000;
    __asm__ volatile ("lui $2,%%hi(D_8009AFB4)\n\taddiu %0,$2,%%lo(D_8009AFB4)" : "=r" (points));
    out = line->xy;
    line->tag = 0x4000000;
    line->code = saved_code | 0x20000000;
    c = cosine;
    s = sine;
    for (; ctr < 3; ctr++) {
        out[0] = (origin + (c * points[0] - s * points[1])) >> 16;
        out[1] = (s * points[0] + c * points[1] + bias) >> 16;
        points += 2;
        out += 2;
    }
}
