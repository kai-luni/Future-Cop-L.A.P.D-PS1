#include "game/types.h"

extern u32 D_8009F6BC;
extern u32 func_80045028(u32 arg);

u8 func_80029BC0(void)
{
    return func_80045028(D_8009F6BC) & 0xFF;
}
