// Categories: Graphics > Effects > Quad Batch | Graphics > GTE
#include "game/types.h"

extern s32 gFrameSwitch __asm__("D_8009FAC8");
extern void gTransformToViewSpace(void *, void *) __asm__("func_800178DC");
extern s32 func_80019E70(s16 *, s32);
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void func_8001047C(void *, void *, void *, s32);
extern void func_80042004(s32, u8 *, u8 *);

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
 * Draws the ten quads of one effect object: it bails out when the
 * overlay/frame switch D_8009FAC8 is 0, fetches the camera-relative transform
 * with func_800178DC, culls with func_80019E70(point, distance << 4)
 * requiring exactly 2, and rejects the back side by the dot product of the
 * point with matrix words 6..8 unless gate byte +0xA1 bit 0 is set. It then
 * loads the matrix, projects fifteen vertices with func_8001047C and emits
 * two quads per loop step through func_80042004, starting at screen x 0xCC00
 * and advancing by 0xC00. It is a 2-D effect draw emitting straight into the
 * packet builder, reached from message 3 (draw) of the object-type-34 handler
 * func_80042330:58, so it is beside the region pass, not inside
 * gDrawViewLayers.
 */
void func_80042128(u8 *object)
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
    if ((u8)func_80019E70(point, *(s16 *)(object + 0xa2) << 4) != 2) return;
    if (!(object[0xa1] & 1)) {
        if (point[0] * matrix[6] + point[1] * matrix[7] + point[2] * matrix[8] > 0) return;
    }
    gGteLoadMatrix(matrix);
    func_8001047C(object + 0xa8, points, flags, 15);
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
