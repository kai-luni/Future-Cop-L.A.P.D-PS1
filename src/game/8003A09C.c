#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Unk11C {
    u32 a;
    u32 b;
    u8 pad[4];
};

struct Script {
    u8 unk00[0x0C];
    struct Cmd commands[4];
    u8 unk14[0x108];
    struct Unk11C unk11C[2];
    u8 unk134[0x0C];
    u8 unk140;
};

extern void func_8002A5B4(struct Script *script, s32 index);

void func_8003A09C(struct Script *script)
{
    s32 i;
    s32 j;
    s32 op;
    s32 arg;

    for (i = 0; i < 4;) {
        op = script->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = script->commands[i].arg;
        switch (op) {
        case 0x6E:
            if (arg == 0) {
                script->unk140 &= 0xFE;
            } else {
                script->unk140 |= 1;
            }
            for (j = 0; j < 2; j++) {
                script->unk11C[j].a = 0;
                script->unk11C[j].b = 0;
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(script, i);
    }
}
