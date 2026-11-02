#include "game/types.h"

extern u32 D_8009F51C;
extern u32 D_8009F520;
extern u8 D_8009C560[];
extern void func_80089A88(u32, u32, void *);

void func_80089A40(void *arg0)
{
    if (arg0 != 0) {
        func_80089A88(D_8009F51C, D_8009F520, arg0);
    } else {
        func_80089A88(D_8009F51C, D_8009F520, D_8009C560);
    }
}
