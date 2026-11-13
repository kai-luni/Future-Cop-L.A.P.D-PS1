#include "game/types.h"

typedef struct { u16 x, y, w, h; } Rect;
typedef struct { s16 x, y, w, h; } Bounds;
typedef struct {
    Bounds rects[2];
    volatile s32 index;
    u8 pad14[8];
    volatile Rect rect;
    s32 dest;
    u8 pad28[0xF4];
    volatile s32 flag;
} State;

extern State *D_8009F4BC;
extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;
extern void func_800847E0(s32, s32, Rect *);
extern void func_80092F38(Rect *, s32);
extern void func_80096070(s32, s32);

void func_80085074(void)
{
    Rect rect;
    State *s;
    if (D_8009F4C0 == 1 && D_8009F4C4 != 0) {
        rect.x = ((s16)D_8009F4BC->rect.x * 2) / 3;
        rect.y = D_8009F4BC->rect.y;
        rect.w = ((s16)D_8009F4BC->rect.w * 2) / 3;
        rect.h = D_8009F4BC->rect.h;
        func_800847E0(D_8009F4BC->dest, D_8009F4BC->dest, &rect);
        func_80092F38(&rect, D_8009F4BC->dest);
    } else {
        func_80092F38(&D_8009F4BC->rect, D_8009F4BC->dest);
    }
    s = D_8009F4BC;
    s->rect.x += D_8009F4C8 * 8;
    if ((s16)s->rect.x < (s16)s->rects[s->index].x + (s16)s->rects[s->index].w) {
        func_80096070(s->dest, ((s16)s->rect.w * (s16)s->rect.h) / 2);
    } else {
        s->flag = 1;
        s->index = !s->index;
        s->rect.x = s->rects[s->index].x;
        s->rect.y = s->rects[s->index].y;
    }
}
