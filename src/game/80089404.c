#include "game/types.h"

struct Sub80089404 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
};

struct State80089404 {
    u8 pad000[0x414];
    u32 unk414;
    u8 pad418[0xC];
    u32 unk424;
    u8 pad428[4];
    struct Sub80089404 *unk42C;
    u8 pad430[0xC];
    u32 unk43C;
    volatile u32 unk440;
    volatile u32 unk444;
    volatile u32 unk448;
};

extern struct State80089404 *D_8009F970;
extern u32 D_8009F980;

extern s32 func_800886C8(s32 delta, s32 value);
extern void func_80089018(void);
extern void func_8008CD10(s32 arg0, s32 arg1);

s32 func_80089404(s32 *out)
{
    u32 i;
    u32 retry;
    s32 available;
    s32 value;
    register s32 offset __asm__("$6");
    register struct State80089404 *state __asm__("$4");
    register s32 base __asm__("$4");
    register u32 old440 __asm__("$5");
    register u32 old444 __asm__("$8");
    register u32 dividend __asm__("$2");
    register u32 divisor __asm__("$9");
    register u32 rem1 __asm__("$3");
    register u32 rem2 __asm__("$7");
    register struct Sub80089404 *sub __asm__("$6");

    i = 0;
    *out = func_800886C8(D_8009F970->unk42C->unk10 - D_8009F970->unk42C->unk04, *out);
    retry = i;

    available = D_8009F970->unk448;
    available = D_8009F970->unk440;
    (void)D_8009F970->unk444;

    for (;;) {
        available = (D_8009F970->unk448 << 11) - ((D_8009F970->unk440 << 11) + D_8009F970->unk444);
        if (available < 0) {
            available = 0;
        }
        if (available >= *out) {
            break;
        }

        if (D_8009F970->unk43C != 0) {
            return 0;
        }

        i++;
        if (i > 0x3D0900U) {
            D_8009F970->unk424 += 2;
        }

        if (D_8009F970->unk414 != 0 && D_8009F970->unk424 < 2) {
            continue;
        }

        retry++;
        if (retry >= 3) {
            return 0;
        }

        func_80089018();
        i = 0;
    }

    state = D_8009F970;
    old440 = state->unk440;
    old444 = state->unk444;
    sub = ((volatile struct State80089404 *)state)->unk42C;
    dividend = state->unk440;
    rem1 = sub->unk34;
    divisor = D_8009F980;
    dividend += rem1;
    rem1 = dividend % divisor;
    rem2 = old440 % divisor;
    dividend = state->unk444;
    base = sub->unk30;
    rem1 <<= 11;
    rem1 += dividend;
    value = base + rem1;
    dividend = divisor - 1;
    dividend -= rem2;
    dividend <<= 11;
    dividend += 0x800;
    offset = dividend - old444;
    rem1 = *out;
    rem1 = (u32)offset < rem1;

    if (rem1) {
        func_8008CD10(value - (D_8009F980 << 11), value);
        value -= D_8009F980 << 11;
    }

    return value;
}
