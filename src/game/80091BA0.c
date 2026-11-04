#include "game/types.h"

extern s32 D_8009DD30[];
extern volatile s32 D_8009DD50;
extern s32 *D_8009DD54;

extern void func_80091C90(s32 *dest, u32 count);
extern void func_800914B0(s32 a0, void *a1);
extern void func_80091BF8(void);
extern void func_80091C64(u32 index, s32 value);

void *func_80091BA0(void)
{
    *D_8009DD54 = 0x107;
    D_8009DD50 = 0;
    func_80091C90(D_8009DD30, 8);
    func_800914B0(0, func_80091BF8);
    return func_80091C64;
}
