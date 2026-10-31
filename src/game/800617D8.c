#include "game/types.h"

extern s32 D_8009F818[2];
extern s32 D_8009F820;
extern s32 D_8009F830;
extern u8 D_800A8DC0[];

extern s32 func_80017EBC(s32, s32);

void func_800617D8(void)
{
    register s32 *p __asm__("$17");
    s32 i;

    D_8009F820 = 0;
    for (i = 0, p = D_8009F818; i < 2; i++) {
        *p = func_80017EBC(0x400, 2);
        p++;
    }
    D_8009F830 = func_80017EBC(0x400, 2);
    for (i = 1; i >= 0; i--) {
        D_800A8DC0[i * 0x44] = 0;
    }
}
