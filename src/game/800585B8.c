// Categories: Gameplay > Entity Script | Game Engine > Command Queue | Gameplay > Player
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

extern void gRemoveEntityCommand(struct Obj58 *self, s32 index) __asm__("func_8002A5B4");

/**
 * @brief Executes and consumes player command opcodes 0x5F..0x62 from the 4-slot queue.
 *
 * Walks the 2-byte {op, arg} slots at self->commands (+0x0C), stopping on opcode 0.
 * Handles 0x5F (clear target lock unk6C), 0x60 (add arg << 7 to unkD0), 0x61
 * (arm state/timers unkA4..unkAE), and 0x62 (set unk2FD when unkC0 == 0).
 *
 * @param self Player entity block (+0x0C commands[4], +0xAC Sub58 pointer).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops i = 0..3 breaking on op == 0; unhandled opcodes advance i++, while handled opcodes 0x5F..0x62 dequeue slot i via func_8002A5B4 without advancing i.
 *
 * @par Calls
 * func_8002A5B4 - shifts later command pairs down and zeroes the trailing opcode byte.
 *
 * @par Side effects
 * Mutates self->commands and Sub58 fields unk6C, unkD0, unkA4..unkAE, or unk2FD inside self->unkAC.
 *
 * @par Callers
 * Confirmed: func_800588D4 (0x80058924, player entity tick when self->unk0C != 0).
 */
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
        gRemoveEntityCommand(self, i);
    }
}
