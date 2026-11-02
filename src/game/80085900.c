#include "game/types.h"

typedef struct {
    u8 pad[0x11C];
    s32 unk11C;
} Unk8009F4BC;

extern Unk8009F4BC *D_8009F4BC;

void func_80085900(s32 arg0)
{
    *(volatile s32 *)&D_8009F4BC->unk11C = arg0;
}
