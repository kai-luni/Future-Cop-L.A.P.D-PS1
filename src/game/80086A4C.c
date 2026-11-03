#include "game/types.h"

typedef struct {
    volatile u32 unk00;
    u32 unk04;
    u8 unk08[0x7B70];
    s32 unk7B78;
} Unk80086A4C;

extern Unk80086A4C *D_8009F4D8;

void *func_80086A4C(u32 arg)
{
    Unk80086A4C *p;

    p = D_8009F4D8;
    return (arg < p->unk04 - p->unk00 && p->unk7B78 == 0)
        ? (u8 *)p + (p->unk00 % 0x3C00 + 0x3C10)
        : 0;
}
