#include "game/types.h"

extern s32 D_8009FAC8;
extern void func_800178DC(void *, void *);
extern s32 func_80019E70(s16 *, s32);
extern void func_80010750(void *);
extern void func_8001047C(void *, void *, void *, s32);
extern void func_80042004(s32, u8 *, u8 *);

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
