#include "game/types.h"

extern u32 D_8009F6DC;
extern void func_80024630(s32 arg0);
extern void func_8002A16C(s32 arg0);

void func_8002A23C(void)
{
    if (D_8009F6DC != 0) {
        func_80024630(0);
        func_8002A16C(1);
    }
}
