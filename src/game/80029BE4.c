#include "game/types.h"

extern u32 D_8009F6DC;
extern u32 func_8002443C(void);

u32 func_80029BE4(void)
{
    u32 value = D_8009F6DC;

    if (value == 3 || value == 4) {
        return 0;
    }
    if (value == 0) {
        return 1;
    }
    return func_8002443C() >= 0x227;
}
