#include "game/types.h"

extern s8 *D_8009F468;
extern void func_800787D8(s32 arg0, s32 arg1, s32 arg2);

void func_80082020(void)
{
    func_800787D8((s32)D_8009F468[0], 0x1, 0x0);
}
