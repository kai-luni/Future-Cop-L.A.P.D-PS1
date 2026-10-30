#include "game/types.h"

u8 *func_80019CA0(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
{
    u8 *p;
    u8 *result;
    s32 found;
    u8 byte_found;

    result = 0;
    p = arg0;
    found = 0;
    while (!found && (u32)(p - arg0) < arg1) {
        result = p;
        byte_found = (*(s32 *)(p + 8) == arg3) && (*(u32 *)result == arg2);
        found = byte_found;
        if (!byte_found) {
            p = result + *(s32 *)(result + 4);
        }
    }
    return result;
}
