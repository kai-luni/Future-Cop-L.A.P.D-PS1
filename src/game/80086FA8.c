#include "game/types.h"

extern u8 *D_8009F4D8;

s32 func_80086FA8(void)
{
    s32 count = 0;
    u32 i = count;
    u8 *ptr = D_8009F4D8;

    do {
        if (*(u32 *)(ptr + 0x7B2C) != 0) {
            count++;
        }
        i++;
        ptr += 4;
    } while (i < 4);

    return count;
}
