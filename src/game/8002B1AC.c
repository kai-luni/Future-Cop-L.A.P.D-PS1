#include "game/types.h"

void func_8002B1AC(u8 *arg0)
{
    s16 old_value = *(s16 *)(arg0 + 0x10C);
    s16 new_value_signed = *(s16 *)(arg0 + 0x158);
    u16 new_value_unsigned = *(u16 *)(arg0 + 0x158);

    if (old_value != new_value_signed) {
        *(u16 *)(arg0 + 0x10C) = new_value_unsigned;
    }

    *(u16 *)(arg0 + 0x10E) = *(u16 *)(arg0 + 0x10C);
}
