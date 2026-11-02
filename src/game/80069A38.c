#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s8 *unkC;
} Unk800A2940;

extern Unk800A2940 D_800A2940;

s32 func_80069A38(s32 *arg0)
{
    register Unk800A2940 *desc __asm__("$6") = &D_800A2940;
    register s32 origin __asm__("$2");
    register s32 width __asm__("$7");
    register s32 dy __asm__("$8");
    register s32 dx __asm__("$9");
    register s32 sy __asm__("$4");
    s32 sx;
    register s32 a __asm__("$5");
    s32 c;
    s32 result;
    s8 *p;

    origin = desc->unk0;
    width = desc->unk4;
    a = arg0[2];
    sx = arg0[0];
    dy = a - origin;
    dx = sx - origin;
    sy = dy >> 14;
    dy &= 0x3FFF;
    sx = dx >> 14;
    dx &= 0x3FFF;
    if (sy < 0) goto bad;
    if (sy > width - 2) goto bad;
    if (sx < 0) goto bad;
    if (sx <= desc->unk8) goto good;

bad:
    result = 0x8000;
    goto done;

good:
    a = sx * width;
    origin = sy + a;
    p = desc->unkC + origin;
    a = p[0] << 8;
    a += (((p[1] << 8) - a) * dy) >> 14;
    p += width;
    c = p[0] << 8;
    c += (((p[1] << 8) - c) * dy) >> 14;
    result = a + (((c - a) * dx) >> 14);
done:
    return result;
}
