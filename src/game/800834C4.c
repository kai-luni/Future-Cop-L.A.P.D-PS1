#include "game/types.h"

extern s32 *D_8009F938;
extern s32 D_8009F93C;
extern s32 D_8009F940;

extern void *func_80017EBC(s32 size, s32 tag);

void func_800834C4(s32 width, s32 height)
{
    s32 *cells;
    s32 count;
    s32 i;

    count = width * height;
    D_8009F940 = height;
    D_8009F93C = width;
    cells = func_80017EBC(count * 8, 2);
    D_8009F938 = cells;
    for (i = count - 1; i >= 0; i--) {
        cells[i * 2] = 0;
        cells[i * 2 + 1] = 0;
    }
}
