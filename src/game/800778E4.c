#include "game/types.h"

extern volatile s32 D_8009FA2C;
extern s32 D_8009F870;
extern s32 D_8009F8E8;
extern u8 D_8009F46C;
extern u8 D_800A3808[8];

extern void func_80078650(u8 *arg0, s32 arg1);
extern void func_800788B4(void);

void func_800778E4(s32 arg0)
{
    if (arg0 != 0) {
        return;
    }

    D_8009F870 = D_8009FA2C + 0x2328;
    if (D_8009F8E8 < D_8009FA2C) {
        func_80078650(D_800A3808, 0);
        D_8009F46C = 1;
        func_800788B4();
    }
}

void func_80077944(s32 arg0)
{
    if (arg0 == 3 && D_8009F46C != 0) {
        D_8009F8E8 = D_8009FA2C + 1;
        func_800778E4(0);
    }
}
