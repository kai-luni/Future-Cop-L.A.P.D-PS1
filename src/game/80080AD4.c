#include "game/types.h"

extern u8 *D_8009F92C;
extern void func_80080984(u8 *arg0, s32 arg1);

void func_80080AD4(void)
{
    u8 *base = D_8009F92C;

    if (base[0x923] != 0) {
        func_80080984(base + 0x40C, 1);
    }
}
