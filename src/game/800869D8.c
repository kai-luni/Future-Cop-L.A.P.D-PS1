#include "game/types.h"

typedef struct {
    volatile u32 start;
    u32 end;
    u8 unk08[0x7B70];
    u32 unk7B78;
} Unk800869D8;

extern Unk800869D8 *D_8009F4D8;

void *func_800869D8(u32 index)
{
    Unk800869D8 *p = D_8009F4D8;
    return (index < p->end - p->start && p->unk7B78 == 0) ? (u8 *)p + ((p->start % 15360) + 0x10) : 0;
}
