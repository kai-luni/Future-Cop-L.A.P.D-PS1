#include "game/types.h"

u8 *func_80019C38(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
{
    u8 *p;
    s32 found;
    u8 byte_found;

    p = arg0;
    if (arg3 == 0) {
        return 0;
    }

    found = 0;
    while (!found && (u32)(p - arg0) < arg1) {
        byte_found = (*(s32 *)(p + 8) == arg3) && (*(u32 *)p == arg2);
        found = byte_found;
        if (!byte_found) {
            p += *(s32 *)(p + 4);
        }
    }

    return p + 0xC;
}
