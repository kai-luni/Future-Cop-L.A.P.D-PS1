#include "game/types.h"

extern volatile s32 D_8009F6D8;
extern s32 D_800A0C60[];

extern void func_80029650(void);

void func_8002A31C(s32 arg0, s32 arg1)
{
    if (D_800A0C60[D_8009F6D8] != 0) {
        func_80029650();
    } else {
        D_800A0C60[D_8009F6D8] = arg0;
        D_8009F6D8 = D_8009F6D8 + 1;
        if (D_8009F6D8 >= 4) {
            D_8009F6D8 = 0;
        }
    }
}
