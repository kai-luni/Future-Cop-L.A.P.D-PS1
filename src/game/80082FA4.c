#include "game/types.h"

struct Poly {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};
extern struct Poly *D_8009F9F0;
extern struct Poly *D_8009FA28;

void func_80082FA4(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b, s32 code)
{
    struct Poly *p;
    s32 x0 = x;
    s32 x1 = x0 + w;
    s32 y2 = y + h;

    __asm__("" : : "r"(x1));

    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    p = D_8009F9F0;
    goto L_done;
L_alloc:
    p = D_8009F9F0;
    D_8009F9F0 = (struct Poly *)((u8 *)p + 0x34);
L_done:
    p->len = 5;
    p->code = code | 0x28;
    p->r = r;
    p->g = g;
    p->b = b;
    p->xy[0] = x0;
    p->xy[1] = y;
    p->xy[2] = x1;
    p->xy[3] = y;
    p->xy[4] = x0;
    p->xy[5] = y2;
    p->xy[6] = x1;
    p->xy[7] = y2;
    p->addr = link;
}
