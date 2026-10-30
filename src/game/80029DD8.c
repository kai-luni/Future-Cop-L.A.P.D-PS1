#include "game/types.h"

extern u8 D_8009F69C;
extern u8 D_8009F6B8;
extern u8 D_8009F6C0;
extern s32 D_8009F6C4;

extern void func_80023E84(void);

void func_80029DD8(void)
{
    u8 flag;

    if (D_8009F69C == 0) {
        D_8009F69C = 1;
        func_80023E84();
        flag = D_8009F6B8;
        D_8009F6C4 = 0;
        D_8009F6C0 = 0;
        D_8009F6B8 = (flag & 0xDF) | 0x8;
    }
}
