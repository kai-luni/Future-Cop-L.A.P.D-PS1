#include "game/types.h"

extern u32 func_8008F8E8(void);
extern u32 func_8008F7AC(void);

u32 func_8008DF30(void)
{
    if (func_8008F8E8() != 0)
        return 0;
    return func_8008F7AC() == 0;
}
