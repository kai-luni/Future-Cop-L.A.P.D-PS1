#include "game/types.h"

extern s32 D_8009B1A4[];
extern s32 *D_8009F9E4;
struct Table { s8 pad[4]; s32 entries[100]; };
extern struct Table D_800A6F38;
extern s32 D_8009FA08;
extern s16 D_8009F83C;
extern void func_800654F8(s32);
extern s32 func_80065744(s32);
extern void func_800646BC(void *, s32, void *, s32);
extern void func_80065134(void *, s32, s32, s32, s32);

void func_8006531C(s32 x, s32 y, s32 *region)
{
    s32 *src = D_8009B1A4;
    s32 *dst = (s32 *)0x1F800000;
    s32 base = D_8009FA08;
    s32 dx = (D_8009F9E4[2] >> 9) - (x << 8);
    s32 dy = (D_8009F9E4[0] >> 9) - (y << 8);
    s32 rows, cols, a, b, hi;
    s32 id;
    register s32 value __asm__("$2");
    register s32 mask __asm__("$18");
    register s8 *obj __asm__("$19");
    D_800A6F38.pad[1] = -1;
    do { *dst++ = *src++; } while ((s32)dst <= 0x1F80006F);
    for (rows = 5; rows > 0; rows--) {
        for (cols = 5; cols > 0; cols--) {
            value = region[0];
            __asm__ volatile ("" : "+r"(value));
            if (value != 0) {
                obj = (s8 *)region[1];
                mask = value;
                id = obj[0x36F];
                if (id != -1 && id != D_800A6F38.pad[1]) {
                    D_800A6F38.pad[1] = id;
                    func_800654F8(D_800A6F38.entries[id]);
                }
                hi = func_80065744(obj[0x3C9]);
                D_8009F83C = (hi << 8) | func_80065744(obj[0x3C8]);
                func_800646BC(obj, base, region + 2, mask << 16);
                for (a = 0xC0; a >= 0; a -= 0x40) {
                    for (b = 0xC; b >= 0; b -= 4) {
                        if (mask & 1) func_80065134(obj, base, dx, dy, a + b);
                        mask >>= 1;
                    }
                }
            }
            dx -= 0x100;
            region += 4;
        }
        dx += 0x500;
        dy -= 0x100;
    }
}
