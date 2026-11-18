#include "game/types.h"

struct UnkF04 {
    u8 unk00[0x18];
    void *f18;
};

struct Sub {
    u8 unk00[4];
    struct UnkF04 *f04;
    u8 *f08;
};

struct Unk28 {
    u8 unk00[0x10];
    void *f10;
    u8 unk14[0x20];
    void *f34;
};

struct State {
    u8 unk00[0x10];
    u8 unk10[0x6];
    u8 b16;
    u8 unk17[0x11];
    struct Unk28 *f28;
    u8 f2C[0x24];
    u8 f50[0xC];
    u8 f5C[0x14];
    s16 h70;
    u8 unk72[0x2E];
    u8 fA0[0x38];
    u8 fDC[0x3C];
    u32 f118;
    u8 unk11C[0x3C];
    u8 f158;
    u8 f159;
    u8 f15A;
};

extern void func_8002AD58(s32 message, void *object, void *data);
extern void func_80060668(void *in, void *out);
extern void func_80050E0C(u8 *a0, u8 *a1, void *a2, struct State *a3, u8 *a4);
extern void func_80037EB4(struct State *state);
extern void func_80044988(void *data, void *track, u8 forward, s32 index);
extern void func_80010750(void *matrix);
extern void func_80010234(void *frame, void *output, s32 count);
extern s32 func_8006314C(s32 key, u8 value);

void func_800378BC(s32 message, struct State *state, struct Sub *sub)
{
    u8 *s2;
    struct Unk28 *f28;
    s32 index;

    func_8002AD58(message, state, sub);
    s2 = sub->f08;
    func_80060668(s2 + 0x24, state->fA0);
    func_80050E0C(s2 + 0x34, state->fDC, sub->f04->f18, state, state->f50);
    func_80037EB4(state);
    f28 = state->f28;
    state->f15A = 0;
    if (f28->f10 != 0) {
        index = 0;
        if (state->b16 == 5) {
            state->f158 = s2[0x4A];
            state->f159 = s2[0x49] | 1;
            index = s2[0x48];
        }
        func_80044988(state->f28->f10, state->fC8, 1, index);
    }
    func_80010750(state->f2C);
    func_80010234((u8 *)state->f28->f34 + ((s32)state->h70 << 4) + 8, state->f5C, 1);
    if (state->f118 & 0x4000) {
        func_8006314C((s32)state, 0);
    }
}
