#include "game/types.h"

struct Command {
    u8 op;
    u8 arg;
};

struct Unk28 {
    u8 unk00[0x34];
    void *unk34;
};

struct Unk41728 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0x14];
    struct Unk28 *unk28;
    u8 unk2C[0x24];
    u8 unk50[0x04];
    u32 unk54;
    u8 unk58[0x04];
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72[0x2E];
    void *unkA0[4];
    u8 unkB0[0x10];
    s32 unkC0;
    u8 unkC4[2];
    u8 unkC6;
    u8 unkC7[2];
    s8 unkC9;
    u8 unkCA;
    u8 unkCB;
};

extern void func_80010750(void *matrix);
extern void func_80010234(void *frame, void *out, s32 count);
extern void func_8002A5B4(void *script, s32 index);

void func_80041728(struct Unk41728 *self)
{
    s32 i;
    s32 op;
    s32 arg;
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");
    register u8 *out __asm__("$5");
    register s32 cnt __asm__("$6");
    s8 cur;
    u8 val;

    for (i = 0; i < 4;) {
        op = self->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = self->commands[i].arg;
        switch (op) {
        case 0x53:
            if (arg == 0) {
                self->unkC6 |= 2;
            } else {
                self->unkC6 &= 0xFD;
            }
            break;
        case 0x52:
            if (arg == 0) {
                goto set_c0;
            }
            if (arg < 11) {
                goto small;
            }
            arg -= 11;
            self->unkCA = arg;
            self->unkC0 = -1;
            self->unk54 = self->unkA0[arg];
            func_80010750(self->unk2C);
            out = self->unk5C;
            cnt = 1;
            offset = self->unk70;
            pointer = (u8 *)self->unk28;
            offset <<= 4;
            __asm__("" : "=r"(offset) : "0"(offset));
            pointer = ((struct Unk28 *)pointer)->unk34;
            offset += 8;
            func_80010234(pointer + offset, out, cnt);
            goto done;
        small:
            if (self->unkC0 <= 0) {
                goto done;
            }
            arg--;
            cur = self->unkC9;
            if (cur == arg) {
                goto done;
            }
            val = 1;
            self->unkCA = arg;
            if (arg < cur) {
                val = 2;
            }
            self->unkCB = val;
            goto set_c0;
        set_c0:
            self->unkC0 = -1;
        done:
            break;
        default:
            goto unknown;
        }
        func_8002A5B4(self, i);
        continue;
    unknown:
        i++;
    }
}
