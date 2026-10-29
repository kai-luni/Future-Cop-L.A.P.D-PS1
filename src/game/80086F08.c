#include "game/types.h"

struct D_8009F4D8_Obj {
    u8 pad_00[0x7B74];
    volatile u32 field_7B74;
};

extern struct D_8009F4D8_Obj *D_8009F4D8;

void func_80086F08(u32 value)
{
    D_8009F4D8->field_7B74 = value;
}
