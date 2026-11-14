#include "game/types.h"

extern u8 D_8009F3E8;

extern void func_8007F5D0(void);

void func_8007F648(s32 a0)
{
    if (a0 == 3) {
        D_8009F3E8 = 1;
        func_8007F5D0();
    }
}
