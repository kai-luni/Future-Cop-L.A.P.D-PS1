#include "game/types.h"

extern s32 *D_8009F4BC;

void func_800858F4(s32 value)
{
    *(s32 *)((u8 *)D_8009F4BC + 0x114) = value;
}
