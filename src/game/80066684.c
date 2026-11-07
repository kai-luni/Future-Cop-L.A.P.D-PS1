#include "game/types.h"

typedef struct { s16 x, y, z; } Vec;
extern u8 D_8009FAA4;
extern s32 D_800A6F60[];
extern void func_8001484C(Vec *, s32, s32, s32, s32, s32, s32);

void func_80066684(u8 *obj, s32 *pos, s32 width, s32 height)
{
    u32 flags;
    s32 i, x, y, scale, offset;
    register u32 tmp __asm__("$2");
    register u32 q __asm__("$3");
    register u8 *p __asm__("$16");
    Vec v;
    if (!obj) return;
    flags = *(u32 *)(obj + 0xa4);
    if (!(flags & 0x492492)) return;
    i = 0;
    p = obj;
    __asm__("" : "+r"(p));
    v.z = 0x1000;
    scale = D_8009FAA4 * 2;
    offset = scale * 103;
    width += offset;
    height += offset;
    for (; i < 8; flags >>= 3, i++, p += 4) {
        if (flags & 2) {
            x = (*(s16 *)(p + 0xb0) << 9) - pos[0];
            if (x + offset >= 0 && x < width) {
                tmp = x * 5;
                __asm__("" : "+r"(tmp));
                q = tmp >> 12;
                __asm__("" : "+r"(q));
                v.x = q;
                y = (*(s16 *)(p + 0xb2) << 9) - pos[1];
                if (y + offset >= 0 && y < height) {
                    tmp = y * 5;
                    __asm__("" : "+r"(tmp));
                    q = tmp >> 12;
                    __asm__("" : "+r"(q));
                    v.y = q;
                    func_8001484C(&v, D_800A6F60[(obj + i)[0xa8] & 31] << 8, 0, 32, scale, 0xe00, 0);
                }
            }
        }
    }
}
