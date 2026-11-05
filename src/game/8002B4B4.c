#include "game/types.h"

struct Command {
    u8 op;
    u8 arg;
};

struct Unk2B4B4 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0x14B];
    u8 unk15F;
    u8 unk160;
};

extern void func_8002B1DC(struct Unk2B4B4 *self, s32 arg, s32 zero);
extern void func_8002B1AC(struct Unk2B4B4 *self);
extern void func_8002A5B4(struct Unk2B4B4 *self, s32 index);

void func_8002B4B4(struct Unk2B4B4 *self)
{
    s32 i;
    s32 op;
    s32 arg;

    for (i = 0; i < 4;) {
        op = self->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = self->commands[i].arg;
        switch (op) {
        case 0x5A:
            self->unk15F = arg;
            func_8002B1DC(self, arg, 0);
            break;
        case 0x5B:
            if (arg != 0) {
                self->unk160 &= 0xFE;
            } else {
                self->unk160 |= 0xFE;
                func_8002B1AC(self);
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
