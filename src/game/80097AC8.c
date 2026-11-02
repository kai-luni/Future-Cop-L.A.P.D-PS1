#include "game/types.h"

extern s32 D_8009EECC;
extern s32 D_8009EF04;
extern s32 D_8009EF08;
extern u8 *D_8009EF0C;

s32 func_80097AC8(s32 count, u8 *table)
{
    s32 shift;

    if (count > 0) {
        shift = D_8009EECC;
        ((u32 *)table)[0] = 0x40001010;
        D_8009EF0C = table;
        D_8009EF08 = 0;
        D_8009EF04 = count;
        ((u32 *)table)[1] = (0x10000 << shift) - 0x1010;
        return count;
    }

    return 0;
}
