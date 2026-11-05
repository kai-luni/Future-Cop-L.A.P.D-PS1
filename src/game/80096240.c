#include "game/types.h"

extern u32 *D_8009E9F8;
extern u32 *D_8009E9C0;
extern u32 *D_8009E9C4;
extern u32 *D_8009E9F0;
extern u32 *D_8009E9C8;
extern s32 func_8009635C(void);

void func_80096240(u32 *arg0, u32 arg1)
{
    func_8009635C();
    *D_8009E9F8 |= 0x88;
    *D_8009E9C0 = (u32)(arg0 + 1);
    *D_8009E9C4 = ((arg1 >> 5) << 16) | 0x20;
    *D_8009E9F0 = *arg0;
    *D_8009E9C8 = 0x01000201;
}
