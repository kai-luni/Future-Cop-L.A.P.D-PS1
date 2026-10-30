#include "game/types.h"

typedef struct {
    u8 pad00[0x14];
    s32 count;
    u8 pad18[0x2C];
    struct {
        s32 field;
        u8 pad[0x20];
    } array[1];
} Unk800A0968;

extern Unk800A0968 D_800A0968;

s32 func_80022C68(void)
{
    s32 i;
    s32 ret;

    i = 0;
    ret = i;

    for (; i < D_800A0968.count; i++) {
        D_800A0968.array[i].field = 0;
    }

    return ret;
}
