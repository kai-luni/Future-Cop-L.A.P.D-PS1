#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect1F90;

extern void func_80092F38(void *rect, void *data);
extern u16 func_80092230(s32 type, s32 blend, s32 x, s32 y);

u16 func_80091F90(void *data, s32 type, s32 blend, s32 x, s32 y, s32 w, s32 h)
{
    Rect1F90 rect;

    rect.x = x;
    rect.y = y;
    rect.h = h;
    switch (type) {
    case 0:
        rect.w = w / 4;
        break;
    case 1:
        rect.w = w / 2;
        break;
    case 2:
        rect.w = w;
        break;
    }
    func_80092F38(&rect, data);
    return func_80092230(type, blend, x, y);
}
