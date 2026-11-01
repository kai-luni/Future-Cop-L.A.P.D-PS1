#include "game/types.h"

extern s32 D_800A09F0;

extern void func_80098A78(s32 mode);

void func_80022F7C(void)
{
    D_800A09F0 = -1;
    func_80098A78(0);
}
