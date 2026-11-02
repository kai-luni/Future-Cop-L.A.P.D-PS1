#include "game/types.h"

extern s32 D_8009F6DC;
extern s32 D_8009F6CC;
extern u8 D_8009F308;
extern s32 D_800A0C50[];
extern void func_8002A31C(s32 arg0, s32 arg1);

void func_8002A49C(void)
{
    s32 temp;

    if (D_8009F6DC != 0) {
        temp = D_800A0C50[D_8009F6CC];
        D_8009F308 = 1;
        while (temp != 0) {
            func_8002A31C(temp, 0);
            D_800A0C50[D_8009F6CC] = 0;
            D_8009F6CC = D_8009F6CC + 1;
            if (D_8009F6CC >= 4) {
                D_8009F6CC = 0;
            }
            temp = D_800A0C50[D_8009F6CC];
        }
        D_8009F308 = 0;
    }
}
