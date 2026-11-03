#include "game/types.h"

struct Model;
struct ModelInstance;

struct Command {
    u8 op;
    u8 arg;
};

struct Unk2CB20 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[2];
    u8 unk16;
    u8 unk17[0x0D];
    u32 options;
    u8 unk28[0xB4];
    s16 unkDC;
    u8 unkDE;
    u8 unkDF;
    u8 unkE0[0x28];
    struct Model *models[4];
};

extern void func_80013AF0(void *instance, struct Model *model, u32 options);
extern void func_8002A5B4(struct Unk2CB20 *self, s32 index);

void func_8002CB20(struct Unk2CB20 *self)
{
    s32 i;
    s32 op;
    u32 arg;

    for (i = 0; i < 4;) {
        op = self->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = self->commands[i].arg;
        switch (op) {
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
            self->unkDE = op - 0x28;
            self->unkDC = arg;
            break;
        case 0x2D:
            if (arg != 0) {
                self->unkDF &= 0xFB;
            } else {
                self->unkDF |= 4;
            }
            break;
        case 0x2E:
            if (self->unk16 == 0x1E && arg < 4) {
                func_80013AF0(&self->unk28, self->models[arg], self->options);
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
