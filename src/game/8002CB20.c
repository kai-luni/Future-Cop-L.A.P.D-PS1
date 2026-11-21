// Categories: Graphics > Model Renderer > Script | Graphics > Animation
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

extern void gInitModelInstance(void *instance, struct Model *model, u32 options) __asm__("func_80013AF0");
extern void gRemoveEntityCommand(struct Unk2CB20 *self, s32 index) __asm__("func_8002A5B4");
void gDispatchModelVariantCommands(struct Unk2CB20 *self) __asm__("func_8002CB20");


/**
 * @brief Interprets model-switch script commands: variant select, a flag
 *        toggle and a model rebind.
 *
 * @param self Script owner; +0xC four {op, arg} commands, +0x16 a type
 *        byte, +0x24 the options word, +0xDC..+0xDF state, +0x108 four
 *        model pointers.
 * @return Nothing.
 *
 * @par Confirmed
 * Ops 0x29..0x2C store unkDE = op - 0x28 (variant 1..4) and unkDC = arg;
 * op 0x2D clears bit 2 of unkDF for non-zero arg, sets it for 0; op 0x2E
 * rebinds the instance at +0x28 via func_80013AF0(models[arg], options)
 * only when unk16 == 0x1E and arg < 4. Executed commands are consumed by
 * func_8002A5B4; op 0 ends, unknown ops skip.
 *
 * @par Likely role
 * The appearance-script pump behind func_8002CC30: the type gate 0x1E
 * restricts model swaps to one object class, the variant words likely
 * select paint/loadout options.
 *
 * @par Unknown
 * What the variant state +0xDC/+0xDE drives downstream.
 *
 * @par Callers
 * func_8002CC30.
 */
void gDispatchModelVariantCommands(struct Unk2CB20 *self)
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
                gInitModelInstance(&self->unk28, self->models[arg], self->options);
            }
            break;
        default:
            i++;
            continue;
        }
        gRemoveEntityCommand(self, i);
    }
}
