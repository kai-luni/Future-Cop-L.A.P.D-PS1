#include "game/types.h"

extern s32 *D_8009F930;
extern void *D_8009F934;
extern s32 D_8009F938;
extern s32 D_8009F944;

extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_800194DC(s32 count, s32 size, s32 tag);

void func_8008347C(void)
{
    s32 *p;

    D_8009F944 = 0;
    D_8009F938 = 0;
    D_8009F934 = func_800194DC(0xFA, 0x5C, 2);
    p = func_80017EBC(0x104, 2);
    D_8009F930 = p;
    *p = -1;
}
