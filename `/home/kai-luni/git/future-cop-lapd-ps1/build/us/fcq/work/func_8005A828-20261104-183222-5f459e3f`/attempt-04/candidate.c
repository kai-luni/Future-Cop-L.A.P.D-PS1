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
    s32 sine;
    s32 cosine;
    s32 i;
    s32 bias;
    s16 *out;
    s16 *points;

    func_8001846C(angle, &sine, &cosine);
    line = (struct LineF3 *)D_8009FA90;
    if (D_8009FA90 < D_8009F9C0) {
        D_8009FA90 += 0x1C;
    }
    i = 0;
    bias = 0x2C0000;
    points = D_8009AFB4;
    out = line->xy;
    line->tag = 0x4000000;
    line->code = code | 0x20000000;
    for (; i < 3; i++) {
        out[0] = (origin + cosine * points[0] - sine * points[1]) >> 16;
        out[1] = (sine * points[0] + cosine * points[1] + bias) >> 16;
        points += 2;
        out += 2;
    }
}
