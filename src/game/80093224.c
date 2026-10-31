#include "game/types.h"

typedef struct {
    u8 unk00[0x14];
    void (*unk14)(u8 *, s32);
    u8 unk18[0x24];
    void (*unk3C)(s32);
} Unk8009DDE8;

extern Unk8009DDE8 *D_8009DDE8;

void func_80093224(u8 *arg0)
{
    s32 temp = arg0[3];

    D_8009DDE8->unk3C(0);
    D_8009DDE8->unk14(arg0 + 4, temp);
}
