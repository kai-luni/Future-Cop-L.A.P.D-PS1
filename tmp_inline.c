
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

static __inline__ struct Poly *alloc_poly(void) {
    struct Poly *p = D_8009F9F0;
    if (p < D_8009FA28) {
        D_8009F9F0 = (struct Poly *)((u8 *)p + 0x34);
    }
    return p;
}

void func_80082EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b)
{
    s32 x0 = x;
    s32 x1 = x0 + w;
    struct Poly *p;

    p = alloc_poly();
    h = y + h;

    p->len = 5;
    p->code = 0x28;
    p->r = r;
    p->g = g;
    p->b = b;
    p->xy[0] = x0;
    p->xy[1] = y;
    p->xy[2] = x1;
    p->xy[3] = y;
    p->xy[4] = x0;
    p->xy[5] = h;
    p->xy[6] = x1;
    p->xy[7] = h;
    p->addr = link;
}
