#include "game/types.h"

extern void *D_8009F948;
extern void *D_8009F94C;
extern void *D_8009F950;
extern void *D_8009F954;

extern void *func_80017EBC(s32 size, s32 tag);

void func_800860CC(void)
{
    D_8009F948 = (void *)0;
    D_8009F94C = (void *)0;
    D_8009F950 = (void *)0;
    D_8009F954 = (void *)0;

    D_8009F948 = func_80017EBC(0x6000, 2);
    D_8009F94C = func_80017EBC(0x600, 2);
    D_8009F954 = func_80017EBC(0x2000, 2);
    D_8009F950 = func_80017EBC(0x600, 2);
}
