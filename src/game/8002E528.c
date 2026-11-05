#include "game/types.h"

struct Command {
    u8 op;
    u8 arg;
};

struct Unk2E528 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0xC8];
    u8 state;
    u8 unkDD[9];
    u8 unkE6;
};

extern void func_8002E2BC(struct Unk2E528 *self, s32 state);
extern void func_8002A5B4(struct Unk2E528 *self, s32 index);

void func_8002E528(struct Unk2E528 *self)
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
        if (op != 0x55) {
            if (op != 0x56) goto other;
            self->unkE6 = arg;
            goto consume;
        }
            if (arg == 0) {
                switch (self->state) {
                case 1:
                case 2:
                    func_8002E2BC(self, 3);
                    break;
                }
            } else {
                switch (self->state) {
                case 0:
                case 3:
                    func_8002E2BC(self, 1);
                    break;
                case 1:
                case 2:
                    break;
                }
            }
        goto consume;
    other:
        i++;
        continue;
    consume:
        func_8002A5B4(self, i);
    }
}
