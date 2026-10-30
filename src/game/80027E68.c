#include "game/types.h"

extern s32 D_8009F9FC;
extern s32 D_8009FA30;
extern s32 D_8009FA60;
extern s32 D_8009FAA4;

extern void func_80057864(void);

void func_80027E68(void)
{
    s32 diff;
    s32 current;

    do {
        func_80057864();
        current = D_8009FA60;
        diff = current - D_8009FA30;
        D_8009FAA4 = current;
    } while (diff < 5);

    if (diff >= 0x12D) {
        diff = 0x12C;
    }

    D_8009F9FC = diff;
    D_8009FA30 = current;
}
