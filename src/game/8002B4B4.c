// Categories: Gameplay > Entity Script | Game Engine > Command Queue | AI > Command Processing
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

/**
 * @brief Executes opcodes 0x5A and 0x5B from a class-26 actor's 4-slot command queue.
 *
 * Iterates commands[0..3] at self+0x0C, stopping at opcode 0. Opcode 0x5A writes self->unk15F = arg and calls func_8002B1DC(self, arg, 0); opcode 0x5B clears bit 0 of unk160 when arg != 0 or sets unk160 |= 0xFE and calls func_8002B1AC when arg == 0. Consumed slots are removed via func_8002A5B4.
 *
 * @param self Actor struct with 4-slot command queue (+0x0C), state (+0x15F), and flags (+0x160).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops up to 4 slots; breaks on op == 0, skips unhandled opcodes with i++, and compacts 0x5A/0x5B via func_8002A5B4(self, i) without advancing i.
 *
 * @par Calls
 * func_8002B1DC - execute state transition; func_8002B1AC - re-arm +0x10C/+0x10E timers; func_8002A5B4 - remove slot i from queue.
 *
 * @par Side effects
 * Mutates self->unk15F, self->unk160, actor timers, and command queue entries at self+0x0C..+0x13.
 *
 * @par Callers
 * Confirmed: func_8002B784.
 */
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
