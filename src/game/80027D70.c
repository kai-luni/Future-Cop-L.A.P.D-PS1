#include "game/types.h"

extern s32 D_8009FA0C;
extern void func_800486FC(s32 arg);

void func_80027D70(void)
{
    if (D_8009FA0C == 1) {
        func_800486FC(1);
    }
}
