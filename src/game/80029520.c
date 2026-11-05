#include "game/types.h"

typedef struct {
    s32 unk00[2];
    s32 unk08[2];
    u8 count10;
    u8 count11;
    u8 unk12;
    u8 unk13;
} Data800A0C30;

extern Data800A0C30 D_800A0C30;

extern void func_80029650(s32 arg0);

void func_80029520(void)
{
    u32 i;

    for (i = 0; i < D_800A0C30.count10; i++) {
        func_80029650(D_800A0C30.unk00[i]);
    }

    D_800A0C30.count10 = 0;

    for (i = 0; i < D_800A0C30.count11; i++) {
        func_80029650(D_800A0C30.unk08[i]);
    }

    D_800A0C30.count11 = 0;
    D_800A0C30.unk12 = 0;
    D_800A0C30.unk13 = 0xFF;
}
