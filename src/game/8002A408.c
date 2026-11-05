#include "game/types.h"

extern s32 D_8009F6D0;
extern s32 D_800A0C60[];

extern void func_80029650(s32 arg0);

void func_8002A408(void)
{
    s32 temp = D_800A0C60[D_8009F6D0];

    while (temp != 0) {
        func_80029650(temp);
        D_800A0C60[D_8009F6D0] = 0;
        D_8009F6D0 = D_8009F6D0 + 1;
        if (D_8009F6D0 >= 4) {
            D_8009F6D0 = 0;
        }
        temp = D_800A0C60[D_8009F6D0];
    }
}
