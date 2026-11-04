#include "game/types.h"

struct Unk80089304Node {
    u8 pad_00[4];
    s32 unk04;
    u8 pad_08[8];
    s32 unk10;
    u8 pad_14[0x14];
    s32 unk28;
};

struct Unk80089304State {
    u8 pad_000[0x42C];
    struct Unk80089304Node *unk42C;
    struct Unk80089304Node *unk430;
    u8 pad_434[0xC];
    s32 unk440;
    volatile s32 unk444;
    s32 unk448;
    s32 unk44C;
};

extern struct Unk80089304State *D_8009F970;
extern void func_80089018(void);

void func_80089304(s32 arg0)
{
    {
        register s32 amount __asm__("$5");
        register struct Unk80089304Node *node __asm__("$3");
        register s32 rem __asm__("$4");
        struct Unk80089304State *state;
        s32 lim;

        amount = arg0;
        __asm__("" : "+r"(amount));
        state = D_8009F970;
        node = state->unk42C;
        rem = state->unk444;
        __asm__("" : "+r"(node) : "r"(rem));
        node->unk04 += amount;
        lim = 0x800 - rem;

        if (amount < lim) {
            state->unk444 += amount;
        } else {
            register u32 pages __asm__("$2");
            amount -= lim;
            state->unk444 = amount & 0x7FF;
            pages = ((u32)amount >> 11) + 1;
            state->unk440 += pages;
        }
    }
    func_80089018();
    {
        register struct Unk80089304State *state __asm__("$5");
        register struct Unk80089304Node *node __asm__("$4");
        state = D_8009F970;
        node = state->unk42C;
        if (node->unk04 == node->unk10 && state->unk430 != node) {
            if (state->unk444 != 0) {
                state->unk440++;
            }
            D_8009F970->unk44C = 0;
            D_8009F970->unk42C = D_8009F970->unk430;
            D_8009F970->unk444 = D_8009F970->unk42C->unk28;
        }
    }
}
