#include "game/types.h"

extern s32 func_80019668(void);
extern void func_800448D8(void *track, s32 index, s32 direction, s32 flags);

void func_8002B0EC(u8 *arg0, u32 arg1)
{
    u32 v;

    if (arg0[0x15E] != arg1) {
        arg0[0x15E] = arg1;
        arg0[0x162] = (arg1 == arg0[0x164]);
        if (arg1 == 0x3A) {
            v = (u32)func_80019668();
            arg1 = v % 10 + 0x3A;
            if (arg1 == 0x43) {
                arg1 = 0x44;
            }
            if (arg1 - 0x40 < 3 || arg1 == 0x44) {
                *(s32 *)(arg0 + 0x74) = 0;
            }
        }
        func_800448D8(arg0 + 0xC8, arg1, 1, 1);
    }
}
