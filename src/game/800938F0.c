#include "game/types.h"

extern u8 D_8009DE5C[];
extern void func_8008CD10(u8 *arg0, u8 *arg1, u32 arg2);

u8 *func_800938F0(u8 *arg0)
{
    func_8008CD10(arg0, D_8009DE5C, 0x14);
    return arg0;
}
