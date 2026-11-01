#include "game/types.h"

extern u16 *D_8009DD18;

u16 func_800915FC(u16 a0)
{
    u16 old = *D_8009DD18;
    *D_8009DD18 = a0;
    return old;
}
