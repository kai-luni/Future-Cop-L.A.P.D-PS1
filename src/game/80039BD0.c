#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Script {
    u8 unk00[0x0C];
    struct Cmd commands[4];
    u8 unk14[0x10A];
};

extern void func_8002A5B4(struct Script *script, s32 index);

void func_80039BD0(struct Script *script)
{
    s32 i;
    s32 op;
    s32 arg;

    i = 0;
    while (i < 4) {
        op = script->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = script->commands[i].arg;
        if (op != 0x73) {
            i++;
        } else {
            script->unk00[0x11E] = arg;
            func_8002A5B4(script, i);
        }
    }
}
