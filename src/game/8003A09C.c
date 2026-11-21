// Categories: Gameplay > Entity Script | Game Engine > Command Queue | Physics > Force Fields
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

extern void gRemoveEntityCommand(struct Script *script, s32 index) __asm__("func_8002A5B4");

/**
 * @brief Executes and consumes opcode 0x6E toggle commands for a force-field entity.
 *
 * Scans the 4-entry 2-byte command queue at script->commands (+0x0C), stopping on
 * opcode 0; for opcode 0x6E it updates bit 0 of script->unk140, resets the two
 * accumulated force slots unk11C[0..1], and dequeues the command via func_8002A5B4.
 *
 * @param script Force-field entity block (+0x0C commands[4], +0x11C unk11C[2], +0x140 effectFlags).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops i = 0..3 breaking on op == 0; op == 0x6E toggles unk140 bit 0, zeroes unk11C[0..1].a/b, and calls func_8002A5B4 without advancing i; other ops increment i.
 *
 * @par Calls
 * func_8002A5B4 - shifts remaining command slots down and clears the trailing opcode.
 *
 * @par Side effects
 * Modifies script->commands, clears script->unk11C[0..1].a/b, and sets/clears bit 0 of script->unk140.
 *
 * @par Callers
 * Confirmed: func_8003A160 (0x8003A1A0, force-field per-frame tick when command byte +0x0C is non-zero).
 */
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
        gRemoveEntityCommand(script, i);
    }
}
