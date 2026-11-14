#include "game/types.h"

extern s32 D_8009C774;
extern u8 *D_8009C75C;

s32 func_80089E40(s32 arg0)
{
    s32 result;

    if (D_8009C774 == 0) {
        result = 0;
    } else {
        result = D_8009C75C[(arg0 >> 4) * 240 + 0xE8] == 8;
    }
    return result;
}
