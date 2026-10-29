#include "game/types.h"

typedef struct {
    u8 pad_00[0x7B20];
    volatile u32 value;
} D_8009F4D8_Data;

extern D_8009F4D8_Data * const D_8009F4D8;

void func_80086F1C(void)
{
    D_8009F4D8->value = 1;
}

void func_80086F30(void)
{
    D_8009F4D8->value = 0;
}
