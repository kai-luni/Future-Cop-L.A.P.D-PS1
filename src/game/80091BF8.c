#include "game/types.h"

extern volatile s32 D_8009DD50;
extern void (*D_8009DD30[8])(void);

void func_80091BF8(void)
{
    register void (**p)(void) __asm__("$16");
    s32 i;

    D_8009DD50 = D_8009DD50 + 1;
    for (i = 0, p = D_8009DD30; i < 8; i++) {
        if (*p != 0) {
            (*p)();
        }
        p++;
    }
}
