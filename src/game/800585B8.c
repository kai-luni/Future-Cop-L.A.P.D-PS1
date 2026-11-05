#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Sub58 {
    u8 unk00[0x6C];
    s32 unk6C;
    u8 unk70[0x34];
    u8 unkA4;
    u8 unkA5;
    u8 unkA6;
    u8 unkA7;
    u8 pad[2];
    u16 unkAA;
    u16 padAC;
    u16 unkAE;
    u8 unkB0[0x10];
    s8 unkC0;
    u8 unkC1[0x20F];
    u8 unk2D0[0x2D];
    u8 unk2FD;
};

struct Sub58b {
    u8 unk00[0xD0];
    u16 unkD0;
};

struct Obj58 {
    u8 unk00[0x0C];
    struct Cmd commands[4];
    u8 unk14[0x98];
    struct Sub58 *unkAC;
};

extern void func_8002A5B4(struct Obj58 *self, s32 index);

void func_800585B8(struct Obj58 *self)
{
    s32 i;
    s32 op;
    s32 arg;
    struct Sub58 *p;

    p = self->unkAC;
    for (i = 0; i < 4;) {
        op = self->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = self->commands[i].arg;
        switch (op) {
        case 0x5F:
            p->unk6C = 0;
            break;
        case 0x60:
            ((struct Sub58b *)self->unkAC)->unkD0 += arg << 7;
            break;
        case 0x61:
            p->unkA4 = arg;
            p->unkA7 = 1;
            p->unkA6 = 1;
            p->unkAA = 0;
            p->unkAE = 900;
            break;
        case 0x62:
            if (p->unkC0 == 0) {
                p->unk2FD = arg;
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
