#include "game/types.h"

struct PadPort;

extern struct PadPort *(*D_8009C73C)(s32 slot);

s32 func_8008A1C0(s32 slot, u8 *command)
{
    return func_8008B484(D_8009C73C(slot), command);
}
