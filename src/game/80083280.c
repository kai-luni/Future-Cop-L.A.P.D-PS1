#include "game/types.h"

extern volatile s32 D_8009FA2C;
extern u8 D_8009F3F8[8];
extern void func_80082EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b);

void func_80083280(s32 x, s32 y, s32 w, s32 h)
{
    register s32 w1 __asm__("$19") = w + 1;
    s32 x0 = x - (w1 >> 1);
    s32 y0 = y - (h >> 1);
    s32 y1 = y0 + h;
    s32 idx;
    s32 color;
    register s32 t __asm__("$2");
    register u8 *pal __asm__("$3");

    t = D_8009FA2C;
    __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)"
            : "=r"(pal) : "r"(t));
    idx = (t >> 4) & 7;
    color = *(u8 *)(idx + (s32)pal) + 0x80;

    func_80082EF0(x0, y0 - 2, w1, 2, 0x12C, color, color, color);
    func_80082EF0(x0, y0, 2, h, 0x12C, color, color, color);
    func_80082EF0(x0 + w1 - 2, y0, 2, h, 0x12C, color, color, color);
    func_80082EF0(x0, y1, w1, 2, 0x12C, color, color, color);
}
