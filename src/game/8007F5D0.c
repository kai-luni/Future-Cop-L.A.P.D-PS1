#include "game/types.h"

extern u8 D_8009FAC4;
extern u8 D_8009FA10;
extern u8 D_800A3838[];
extern u8 D_800A3808[];
extern void func_80078650(u8 *, s32);

void func_8007F5D0(void)
{
    if (D_8009FAC4 == 0 && D_8009FA10 != 0) {
        func_80078650(D_800A3838, 1);
    } else {
        func_80078650(D_800A3808, 1);
    }
}
