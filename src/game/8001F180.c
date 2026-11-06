#include "game/types.h"

struct Unk8001F180 {
    u8 pad[0x8F4];
    s32 field_8F4;
    s32 field_8F8;
};

extern struct Unk8001F180 *D_8009AB08;

s32 func_8001F180(void)
{
    struct Unk8001F180 *state = D_8009AB08;

    if (state->field_8F4 != 0) goto one;
    if (state->field_8F8 != 0) goto one;
zero:
    return 0;
one:
    return 1;
}
