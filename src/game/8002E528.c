// Categories: Gameplay > Entity Script | Game Engine > Command Queue | Graphics > Animation
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

extern void gSetActorMode(struct Unk2E528 *self, s32 state) __asm__("func_8002E2BC");
extern void gRemoveEntityCommand(struct Unk2E528 *self, s32 index) __asm__("func_8002A5B4");
void gProcessActorCommandQueue(struct Unk2E528 *self) __asm__("func_8002E528");


/**
 * @brief Executes opcodes 0x55 and 0x56 from an animated actor's 4-slot command queue.
 *
 * Scans self->commands[0..3] at +0x0C until opcode 0. Opcode 0x56 writes arg to self->unkE6 (animation rate); opcode 0x55 transitions state via func_8002E2BC(self, 3) when arg == 0 and state is 1 or 2, or func_8002E2BC(self, 1) when arg != 0 and state is 0 or 3.
 *
 * @param self Actor struct with command queue (+0x0C), state (+0xDC), and rate byte unkE6 (+0xE6).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops up to 4 slots; breaks on op == 0, advances i on unhandled opcodes, and compacts opcodes 0x55/0x56 via func_8002A5B4(self, i) without incrementing i.
 *
 * @par Calls
 * func_8002E2BC - switch actor mode to 1 or 3; func_8002A5B4 - remove slot i from command queue.
 *
 * @par Side effects
 * Writes self->unkE6, compacts self->commands, and transitions actor state/animation via func_8002E2BC.
 *
 * @par Callers
 * Confirmed: func_8002E630.
 */
void gProcessActorCommandQueue(struct Unk2E528 *self)
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
                    gSetActorMode(self, 3);
                    break;
                }
            } else {
                switch (self->state) {
                case 0:
                case 3:
                    gSetActorMode(self, 1);
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
        gRemoveEntityCommand(self, i);
    }
}
