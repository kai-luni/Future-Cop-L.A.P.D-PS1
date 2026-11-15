#include "game/types.h"

extern u8 D_8009F26C;

extern void func_80021E1C(void);

void func_80023314(void)
{
    if (D_8009F26C != 0) {
        func_80021E1C();
    }
}
