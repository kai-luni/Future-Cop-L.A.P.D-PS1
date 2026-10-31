#include "game/types.h"

void func_8007F688(u8 *data, u32 len, u32 *sum, u32 *other)
{
    u32 i;

    *sum = 0;
    *other = 0;
    for (i = 0; i < len; i++) {
        *sum += *data + i;
        *other = ((*data ^ *other) << 1) + i;
        data++;
    }
}
