#include "game/types.h"

typedef struct {
    u8 pad_0[0x7B24];
    u8 *field_7B24;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;

u8 *func_80086F44(void)
{
    return D_8009F4D8->field_7B24;
}
