#include "game/types.h"

struct UnkF4BC {
    u8 pad[0x114];
    s32 unk114;
};

extern struct UnkF4BC *D_8009F4BC;

s32 func_800858E0(void)
{
    return D_8009F4BC->unk114;
}
