#include "game/types.h"

typedef struct {
    volatile u32 field0;
    u8 unk04[0x14];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];

s32 func_8001D3A8(u32 index)
{
    s32 value;

    if (index < 4) {
        if (D_800A6D78[index].field0 != 0) {
            value = D_800A6D78[index].field0 & 0xFF;
        } else {
            value = 0xF7;
        }
    } else {
        value = 0xF7;
    }

    if (value < 0) {
        goto store;
    }
    if ((u32)(value - 2) < 2) {
        return -0xF;
    }
store:
    D_800A6D78[index].field0 = 0;
    return value;
}
