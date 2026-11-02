#include "game/types.h"

extern u32 *D_8009E9CC;
extern u32 *D_8009E9D0;
extern u32 *D_8009E9D4;
extern u32 *D_8009E9F8;

extern void func_800963F0(s32 destination, u32 size);

void func_800962D0(s32 destination, u32 size)
{
    func_800963F0(destination, size);
    *D_8009E9F8 |= 0x88;
    *D_8009E9D4 = 0;
    *D_8009E9CC = destination;
    *D_8009E9D0 = ((size >> 5) << 16) | 0x20;
    *D_8009E9D4 = 0x01000200;
}
