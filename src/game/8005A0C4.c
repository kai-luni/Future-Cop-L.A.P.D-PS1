#include "game/types.h"

struct Unk5A190Gauge {
    s32 unk00;
    s32 value;
    s32 unk08;
    s32 limit;
    u8 state;
    u8 unk11[3];
};

void func_8005A0C4(struct Unk5A190Gauge *gauge, s32 value, s32 divisor)
{
    s32 limit;

    if (value < 0) {
        value = 0;
    }
    gauge->unk00 = divisor;
    gauge->value = value;
    limit = divisor / 3;
    gauge->limit = limit;
    gauge->unk08 = 0x600000 / divisor;
    if (value > limit) {
        gauge->state = 2;
    } else if (value > 0) {
        gauge->state = 1;
    } else {
        gauge->state = 0;
    }
}
