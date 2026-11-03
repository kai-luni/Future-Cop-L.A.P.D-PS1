#include "game/types.h"

extern volatile u32 *D_8009DEC4;
extern u32 *D_8009DEC8;

s32 func_80095350(u32 flags)
{
    *D_8009DEC8 = 0x10000007;

    if ((*D_8009DEC4 & 0xFFFFFF) != 2) {
        *D_8009DEC4 = (*D_8009DEC8 & 0x3FFF) | 0xE1001000;
        (void)*D_8009DEC4;
        return 0;
    }

    if ((flags & 8) == 0) {
        return 1;
    }

    *D_8009DEC8 = 0x09000001;
    return 2;
}
