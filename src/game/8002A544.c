#include "game/types.h"

extern s32 D_8009F6DC;
extern u8 D_8009F6B8;
extern u8 D_800A0B90[];
extern void func_8002A49C(void);

void func_8002A544(void)
{
    if (D_8009F6DC != 0) {
        if ((D_8009F6B8 & 0x40) == 0) {
            D_800A0B90[0] = 1;
        }
        func_8002A49C();
        D_8009F6DC = 0;
    }
}
