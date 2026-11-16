// Categories: Gameplay > Entity Script | Game Engine > Command Queue | Gameplay > Entity Update
#include "game/types.h"

struct Command {
    u8 op;
    u8 arg;
};

struct Unk31004 {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0x88];
    u8 *unk9C;
    u8 unkA0;
    u8 unkA1;
    u16 unkA2;
};

extern void func_8002A5B4(struct Unk31004 *self, s32 index);
extern void func_80069D68(u8 *ptr, s32 arg1, s32 arg2, struct Unk31004 *self);

/**
 * @brief Executes opcodes 0x78, 0x79, and 0x7A from a trigger/attachment entity's 4-slot command queue.
 *
 * Scans self->commands[0..3] at +0x0C until opcode 0. Opcode 0x78 sets unkA2 = arg; 0x79 sets unkA1 = arg; 0x7A sets bit 0x10 of unkA0 when arg == 0, or clears bit 0x10 and invokes script self->unk9C if bit 0x20 of unkA0 was set and unk9C is non-empty.
 *
 * @param self Entity struct with commands (+0x0C), script ptr unk9C (+0x9C), unkA0 (+0xA0), unkA1 (+0xA1), unkA2 (+0xA2).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops up to 4 slots; breaks on op == 0, skips other opcodes with i++, and removes 0x78..0x7A via func_8002A5B4(self, i) without advancing i.
 *
 * @par Calls
 * func_80069D68 - execute bytecode script self->unk9C; func_8002A5B4 - remove slot i from command queue.
 *
 * @par Side effects
 * Updates unkA0..unkA2, compacts self->commands, and may execute VM script self->unk9C.
 *
 * @par Callers
 * Confirmed: func_8003151C.
 */
void func_80031004(struct Unk31004 *self)
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
        case 0x78:
            self->unkA2 = arg;
            break;
        case 0x79:
            self->unkA1 = arg;
            break;
        case 0x7A:
            if (arg != 0) {
                u8 flags = self->unkA0;
                self->unkA0 = flags & ~0x10;
                if (flags & 0x20) {
                    u8 *ptr = self->unk9C;
                    if (ptr != 0 && ptr[0] != 0) {
                        func_80069D68(ptr, 0, -1, self);
                    }
                }
            } else {
                self->unkA0 |= 0x10;
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(self, i);
    }
}
