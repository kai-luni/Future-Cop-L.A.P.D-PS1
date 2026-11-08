#include "game/types.h"

extern u8 *D_8009F92C;

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern s32 func_8008CCC0(u8 *arg0);
extern s32 func_800210D0(u8 arg0);

void func_800822CC(u8 *arg0)
{
    s32 count;
    s32 i;
    s16 *dst;
    u8 *src = arg0;

    func_8001E4C0(D_8009F92C + 4, 0, 0x40);
    count = func_8008CCC0(src);
    dst = (s16 *)(D_8009F92C + 4);
    for (i = 0; i < count; i++) {
        *dst = func_800210D0(*arg0);
        arg0++;
        dst++;
    }
}
