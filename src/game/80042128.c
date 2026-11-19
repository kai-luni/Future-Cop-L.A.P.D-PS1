// Categories: Graphics > Effects > Quad Batch | Graphics > GTE
#include "game/types.h"

extern s32 gFrameSwitch __asm__("D_8009FAC8");
extern void gTransformToViewSpace(void *, void *) __asm__("func_800178DC");
extern s32 gTestFrustumMargin(s16 *, s32) __asm__("func_80019E70");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gProjectVertexBatch(void *, void *, void *, s32) __asm__("func_8001047C");
extern void func_80042004(s32, u8 *, u8 *);
void gDrawPortraitScreen(u8 *object) __asm__("func_80042128");


/**
 * @brief Draws the ten quads of an effect object, after a gate and a backface test.
 *
 * @param object Effect record: transform at +0x2C, gate byte at +0xA1, distance
 *        word at +0xA2, vertex block at +0xA8.
 * @return Nothing; returns early when the frame switch D_8009FAC8 is 0, when
 *         func_80019E70(point, distance << 4) does not return exactly 2, or when
 *         the backface test fails.
 *
 * @par Confirmed
 * func_800178DC(object + 0x2C, matrix) fetches the camera-relative transform,
 * func_80019E70 is queried with the object's distance word shifted left by 4, the
 * dot product `point[0] * m6 + point[1] * m7 + point[2] * m8 > 0` rejects the back
 * side, and a loop `for (i = 0; i < 10; i += 2)` calls func_80042004 twice per step
 * (the second with x + 0x20), i.e. ten quads from fifteen projected vertices
 * (func_8001047C with count 15).
 *
 * @par Likely role
 * The screen-facing glow card of an effect: ten Chequerboard quads covering a
 * 0x20-wide band, drawn only when the effect faces the camera.
 *
 * @par Environment path
 * Draw case (message 3 of the type-34 handler func_80042330) of an in-world
 * screen that shows the animated comm portrait: it returns while D_8009FAC8
 * is zero - the per-frame result of func_80029748, which is non-zero only
 * while a portrait animation runs and uploads its current frame to VRAM
 * (0x3C0, 0x1CC) - then builds the camera-relative matrix with func_800178DC,
 * culls with func_80019E70 (radius +0xA2 << 4) and rejects back-facing
 * screens unless +0xA1 bit 0 is set. It projects the fifteen grid vertices at
 * +0xA8 with func_8001047C and emits eight textured quads (two per row, four
 * rows) through func_80042004, whose UVs (v from 0xCC in steps of 0x0C, tpage
 * 0x11F) address exactly that VRAM rectangle.
 */
void gDrawPortraitScreen(u8 *object)
{
    s16 point[3];
    s32 matrix[12];
    u8 points[120];
    u8 flags[16];
    register s32 i __asm__("$18");
    register s32 x __asm__("$19");
    register u8 *p __asm__("$17");
    register u8 *f __asm__("$16");

    if (!gFrameSwitch) return;
    gTransformToViewSpace(object + 0x2c, matrix);
    point[0] = matrix[9] >> 4;
    point[1] = matrix[10] >> 4;
    point[2] = matrix[11] >> 4;
    if ((u8)gTestFrustumMargin(point, *(s16 *)(object + 0xa2) << 4) != 2) return;
    if (!(object[0xa1] & 1)) {
        if (point[0] * matrix[6] + point[1] * matrix[7] + point[2] * matrix[8] > 0) return;
    }
    gGteLoadMatrix(matrix);
    gProjectVertexBatch(object + 0xa8, points, flags, 15);
    p = points;
    f = flags;
    i = 0;
    x = 0xcc00;
    for (; i < 10; i += 2) {
        func_80042004(x, p, f + i);
        p += 8;
        i++;
        func_80042004(x + 0x20, p, f + i);
        x += 0xc00;
        p += 16;
    }
}
