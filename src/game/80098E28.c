#include "game/types.h"

extern s32 D_8009EED8;
extern s32 D_8009EEDC;
extern void func_80097760(s32 arg0, u32 arg1);

u32 func_80098E28(s32 arg0, u32 arg1)
{
    u32 size;

    size = arg1;
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    func_80097760(arg0, size);

    if (D_8009EEDC != 0) {
        return size;
    }
    D_8009EED8 = 0;
    return size;
}
