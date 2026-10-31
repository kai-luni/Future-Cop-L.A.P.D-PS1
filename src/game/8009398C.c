#include "game/types.h"

extern s32 func_80093F78(s16 arg0, s16 arg1);
extern s32 func_80094010(s16 arg0, s16 arg1);

void func_8009398C(u8 *obj, u8 *rect)
{
    *(u8 *)(obj + 3) = 2;
    *(s32 *)(obj + 4) = func_80093F78(*(s16 *)(rect + 0), *(s16 *)(rect + 2));
    *(s32 *)(obj + 8) = func_80094010(
        (s16)(*(u16 *)(rect + 0) + *(u16 *)(rect + 4) - 1),
        (s16)(*(u16 *)(rect + 2) + *(u16 *)(rect + 6) - 1));
}
