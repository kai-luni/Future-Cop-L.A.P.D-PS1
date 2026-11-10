#include "game/types.h"

extern u8 D_8009DE00[];
extern void func_8008CD10(u8 *arg0, u8 *arg1, u32 arg2);

u8 *func_80093488(u8 *arg0)
{
    func_8008CD10(arg0, D_8009DE00, 0x5C);
    return arg0;
}
