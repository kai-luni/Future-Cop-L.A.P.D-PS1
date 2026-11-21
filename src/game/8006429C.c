// Categories: Physics > Collision | Gameplay > Map
#include "game/types.h"

typedef struct { s16 x, y, z; } Vec;
extern void gGteRotateVertexFlipY(Vec *, Vec *) __asm__("func_800640D8");
extern s32 gIsInViewFrustumWithMargin(s16 *, s32) __asm__("func_80063FF8");
extern s32 gBuildFourCellVisibilityMask(Vec *, s32, void *) __asm__("func_8006411C");
s32 gProbeMapCellVisibility(void *object, Vec point) __asm__("func_8006429C");


/**
 * @brief Probes a point against a cell's collision planes with a cascading
 *        bit mask of follow-up tests.
 *
 * @param object Cell owner; +0x374 the s16 data block (data[0] the range
 *        scale, data[1] the y offset, +0x10/+0x20/+0x30/+0x40 the plane
 *        sets).
 * @param point The probe position.
 * @return A bit mask of hit planes (bits 1/2/4/8/0x10 with their shifted
 *         follow-up bits), or 0 when out of range or no plane hit.
 *
 * @par Confirmed
 * The point is offset by (0x1000, data[1], 0x1000), transformed by
 * func_800640D8 and range-tested via func_80063FF8(data[0] << 4). Then
 * func_8006411C probes: the base test at 0x1000; bit 2 triggers a 0x800
 * probe at data+0x10 (result << 4, cleared unless bits 0x1E0 hit); bit 4
 * a z+0x1000 probe at data+0x20 (<< 8, mask 0x1E00); bit 0x10 an
 * x+0x1000 probe at data+0x40 (<< 16, mask 0x1E0000); bit 8 a z-back
 * probe at data+0x30 (<< 12, mask 0x1E000). No bits 0x1E -> 0.
 *
 * @par Likely role
 * The fine collision test behind func_80064460: the cascade tests the
 * neighbouring planes only when the base plane was hit, the shifted bits
 * encode which adjacent plane responded.
 *
 * @par Unknown
 * The plane layout of the four data sets.
 *
 * @par Callers
 * func_80064460.
 */
s32 gProbeMapCellVisibility(void *object, Vec point)
{
    Vec temp, transformed;
    s16 *data = (s16 *)((char *)object + 0x374);
    s32 result;

    temp.x = point.x + 0x1000;
    temp.y = point.y + data[1];
    temp.z = point.z + 0x1000;
    gGteRotateVertexFlipY(&temp, &transformed);
    result = gIsInViewFrustumWithMargin((s16 *)&transformed, data[0] << 4);
    if (result) {
        result |= gBuildFourCellVisibilityMask(&point, 0x1000, data);
        if (result & 2) {
            result |= gBuildFourCellVisibilityMask(&point, 0x800, (char *)data + 0x10) << 4;
            if (!(result & 0x1e0)) result &= ~2;
        }
        point.z += 0x1000;
        if (result & 4) {
            result |= gBuildFourCellVisibilityMask(&point, 0x800, (char *)data + 0x20) << 8;
            if (!(result & 0x1e00)) result &= ~4;
        }
        point.x += 0x1000;
        if (result & 0x10) {
            result |= gBuildFourCellVisibilityMask(&point, 0x800, (char *)data + 0x40) << 16;
            if (!(result & 0x1e0000)) result &= ~0x10;
        }
        point.z -= 0x1000;
        if (result & 8) {
            result |= gBuildFourCellVisibilityMask(&point, 0x800, (char *)data + 0x30) << 12;
            if (!(result & 0x1e000)) result &= ~8;
        }
        if (!(result & 0x1e)) result = 0;
    }
    return result;
}
