#include "game/types.h"

extern s32 D_8009FAB8;
extern s32 D_8009F760;

u8 func_80048790(void)
{
    switch (D_8009FAB8) {
    case 0:
        return 0;
    case 2:
        if (D_8009F760 == 0) {
            __asm__ volatile ("");
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}
