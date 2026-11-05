#include "game/types.h"

extern s32 *D_8009DD60;
extern s32 D_8009DD64[8];

extern void func_80091F38(s32 *destination, u32 count);
extern void func_800914B0(s32 a0, void *a1);
extern void func_80091D0C(void);
extern void func_80091E8C(s32 a0, s32 a1);

void *func_80091CC0(void)
{
    func_80091F38(D_8009DD64, 8);
    *D_8009DD60 = 0;
    func_800914B0(3, func_80091D0C);
    return func_80091E8C;
}
