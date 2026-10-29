#include "game/types.h"

extern void *D_8009F518;
extern void func_800181D4(void *block);

void func_800898E8(void)
{
    if (D_8009F518 != (void *)0) {
        func_800181D4(D_8009F518);
        D_8009F518 = (void *)0;
    }
}
