#include "game/types.h"

struct PadPort;

extern struct PadPort *(*D_8009C73C)(s32 slot);

s32 func_8008A1F8(s32 slot, u8 mode, u8 lock)
{
    return func_8008B5D0(D_8009C73C(slot), mode, lock);
}
