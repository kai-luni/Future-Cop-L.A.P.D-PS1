#include "game/types.h"

extern void func_80096240(u32 *arg0, u16 arg1);

void func_80095FF4(u32 *arg0, s32 arg1)
{
    if (arg1 & 1) {
        *arg0 &= 0xF7FFFFFF;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= 0xFDFFFFFF;
    }

    func_80096240(arg0, *(u16 *)arg0);
}
