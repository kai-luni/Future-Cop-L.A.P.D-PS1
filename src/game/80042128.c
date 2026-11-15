// Categories: Graphics > Effects > Quad Batch | Graphics > GTE
#include "game/types.h"

extern s32 D_8009FAC8;
extern void func_800178DC(void *, void *);
extern s32 func_80019E70(s16 *, s32);
extern void func_80010750(void *);
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

    if (!D_8009FAC8) return;
    func_800178DC(object + 0x2c, matrix);
    point[0] = matrix[9] >> 4;
    point[1] = matrix[10] >> 4;
    point[2] = matrix[11] >> 4;
    if ((u8)func_80019E70(point, *(s16 *)(object + 0xa2) << 4) != 2) return;
    if (!(object[0xa1] & 1)) {
        if (point[0] * matrix[6] + point[1] * matrix[7] + point[2] * matrix[8] > 0) return;
    }
    func_80010750(matrix);
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
