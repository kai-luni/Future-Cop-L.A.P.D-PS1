#include "game/types.h"

struct Unk425BC {
    u8 unk00[0x0C];
    u8 op;
    u8 arg;
    u8 unk0E[0x96];
    u32 unkA4;
    u8 unkA8[8];
};

extern void func_8002A5B4(u8 *script, s32 index);

void func_800425BC(struct Unk425BC *script)
{
    register struct Unk425BC *self __asm__("$18");
    register struct Unk425BC *p __asm__("$17");
    register s32 i __asm__("$16");
    s32 op;
    s32 arg;
    s32 t;
    u8 *q;

    self = script;
    i = 0;
    p = self;
    while (i < 4) {
        op = p->op;
        if (op == 0) {
            break;
        }
        arg = p->arg;
        switch (op) {
        case 0x4B:
            t = arg >> 3;
            if ((u32)t < 8) {
                arg &= 7;
                t = t * 3;
                self->unkA4 = (self->unkA4 & ~(7u << t)) | ((u32)arg << t);
            }
            func_8002A5B4((u8 *)self, i);
            continue;
        case 0x4C:
            t = arg >> 5;
            arg &= 0x1F;
            if ((u32)t < 8) {
                q = (u8 *)self + t;
                q[0xA8] = (q[0xA8] & 0xE0) | arg;
            }
            func_8002A5B4((u8 *)self, i);
            continue;
        default:
            p = (struct Unk425BC *)((u8 *)p + 2);
            i++;
            continue;
        }
    }
}
