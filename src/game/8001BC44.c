#include "game/types.h"

u32 func_8001BC44(u32 r, u32 g, u32 b)
{
    return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}
