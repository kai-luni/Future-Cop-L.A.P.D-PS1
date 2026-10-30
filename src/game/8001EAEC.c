#include "game/types.h"

extern u8 D_800A6FF8[];

u8 func_8001EAEC(s32 arg0)
{
    u8 *base = D_800A6FF8;
    s32 offset;

    if (arg0 >= 0) {
        if (arg0 < 2) {
            offset = arg0 * 0x18;
        } else {
            offset = 0x18;
        }
    } else {
        offset = 0;
    }

    return *(base + offset + 0x16);
}
