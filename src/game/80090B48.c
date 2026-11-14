#include "game/types.h"

struct State {
    s32 count;              /* 0x00 */
    void *buffer;           /* 0x04 */
    volatile s32 unk8;      /* 0x08 */
    s32 mode;               /* 0x0C */
    volatile s32 command;   /* 0x10 */
    volatile s32 unk14;     /* 0x14 */
    s32 unk18;              /* 0x18 */
    u32 time;               /* 0x1C */
    volatile s32 unk20;     /* 0x20 */
    s32 busy;               /* 0x24 */
    s32 callback;           /* 0x28 */
    volatile s32 callback2; /* 0x2C */
    volatile s32 callback3; /* 0x30 */
    volatile s32 unk34;     /* 0x34 */
};

extern struct State D_8009CBD0;
extern s32 D_8009C814;
extern u32 D_8009C810;

extern void func_800908A0(void);
extern s32 func_8008E194(s32);
extern s32 func_8008E1A8(s32);
extern s32 func_8008E5D8(s32);
extern s32 func_8008E2F8(s32, s32);

void func_80090B48(void)
{
    register struct State *state __asm__("$16");

    state = &D_8009CBD0;
    state->unk8 += state->command * 4;
    state->unk14--;
    state->unk20++;
    if (state->unk14 != 0) {
        return;
    }
    func_8008E1A8(state->callback2);
    if (D_8009C814 & 1) {
        func_8008E5D8(state->callback3);
    }
    func_8008E194((s32)func_800908A0);
    func_8008E2F8(9, 0);
    state->busy = 1;
    if (D_8009C810 != 0) {
        ((void (*)(s32, s32))D_8009C810)(2, state->unk34);
    }
}
