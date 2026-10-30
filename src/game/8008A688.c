#include "game/types.h"

extern u8 D_800A3B60[];

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008CA10(s32 arg0, u8 *arg1);
extern void func_8008CA20(s32 arg0, s32 arg1);

void func_8008A688(void)
{
    func_8008C9A0();
    func_8008CA20(3, 1);
    func_8008CA10(2, D_800A3B60);
    func_8008C9B0();
}
