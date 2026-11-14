#include "game/types.h"

extern u8 D_8009F3E8;

extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_8007F5D0(void);

void func_8007F618(void)
{
    func_80046C84(0x49, 0);
    D_8009F3E8 = 0;
    func_8007F5D0();
}
