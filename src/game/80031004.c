#include "game/types.h"

struct Command {
    u8 op;
    u8 arg;
};

struct Unk31004 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0x88];
    u8 *unk9C;
    u8 unkA0;
    u8 unkA1;
    u16 unkA2;
};

extern void func_8002A5B4(struct Unk31004 *self, s32 index);
extern void func_80069D68(u8 *ptr, s32 arg1, s32 arg2, struct Unk31004 *self);

void func_80031004(struct Unk31004 *self)
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
        case 0x78:
            self->unkA2 = arg;
            break;
        case 0x79:
            self->unkA1 = arg;
            break;
        case 0x7A:
            if (arg != 0) {
                u8 flags = self->unkA0;
                self->unkA0 = flags & ~0x10;
                if (flags & 0x20) {
                    u8 *ptr = self->unk9C;
                    if (ptr != 0 && ptr[0] != 0) {
                        func_80069D68(ptr, 0, -1, self);
                    }
                }
            } else {
                self->unkA0 |= 0x10;
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
