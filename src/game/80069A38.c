// Categories: Utilities > Fixed Point | Utilities > Heightfield Sampling
#include "game/types.h"

extern int gGuidedProjectileUpdate();

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s8 *unkC;
} Unk800A2940;

extern Unk800A2940 gHeightGridDesc __asm__("D_800A2940");

s32 gSampleHeightGridBilinear(s32 *arg0) __asm__("func_80069A38");

/**
 * @brief Bilinear sample of a byte heightfield at a 16.14 fixed-point position.
 *
 * The map descriptor D_800A2940 holds the origin, the row width, the last
 * valid column and the base pointer. The two coordinates of arg0 are split
 * into a cell index (>> 14) and a 14-bit fraction; the sample is the bilinear
 * blend of the four surrounding bytes. Rows and columns are bounds-checked
 * against the descriptor, and out-of-range positions return the sentinel
 * 0x8000 instead of a value.
 *
 * @param arg0 Two 16.14 coordinates: arg0[0] is the column, arg0[2] the row.
 * @return Interpolated height in 8.8 scale, or 0x8000 when outside the map.
 *
 * @par Globals
 * D_800A2940 - origin (0x0), row width (0x4), last column (0x8), base (0xC).
 *
 * @par Control flow
 * Index split, four range tests, then two row interpolations and one column blend.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_800324E0, gGuidedProjectileUpdate.
 */
s32 gSampleHeightGridBilinear(s32 *arg0)
{
    register Unk800A2940 *desc __asm__("$6") = &gHeightGridDesc;
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
