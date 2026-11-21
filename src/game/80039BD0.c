// Categories: Gameplay > Entity Script | Game Engine > Command Queue
#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Script {
    u8 unk00[0x0C];
    struct Cmd commands[4];
    u8 unk14[0x10A];
};

extern void gRemoveEntityCommand(struct Script *script, s32 index) __asm__("func_8002A5B4");

/**
 * @brief Drains opcode 0x73 entries from an entity's 4-slot command queue (+0x0C..+0x13) into byte +0x11E.
 *
 * Scans `script->commands[0..3]` until `op == 0` or slot 4: whenever `op == 0x73`, writes `arg` to `script->unk00[0x11E]` (`self->bytes[0]` in func_80039C58) and removes the slot with func_8002A5B4 without advancing `i` so shifted commands are checked next. Category Gameplay > Entity Script | Game Engine > Command Queue follows queue compaction and caller func_80039C58.
 *
 * @param script Entity script block carrying four `{op, arg}` slots at `+0x0C..+0x13` and byte `+0x11E`.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * `while (i < 4)` breaking on `op == 0`; `op == 0x73` updates `+0x11E` and compacts via func_8002A5B4, other opcodes increment `i`.
 *
 * @par Calls
 * func_8002A5B4 - removes slot `i` by shifting remaining command pairs down and zeroing slot 3's opcode.
 *
 * @par Side effects
 * Updates byte `+0x11E` of `script` and compacts handled `0x73` commands out of `+0x0C..+0x13`.
 *
 * @par Callers
 * Confirmed: func_80039C58 (0x80039D98 in asm/us/main.s, case 6 when `self->command != 0`) / Likely: none / Unknown: none.
 */
void func_80039BD0(struct Script *script)
{
    s32 i;
    s32 op;
    s32 arg;

    i = 0;
    while (i < 4) {
        op = script->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = script->commands[i].arg;
        if (op != 0x73) {
            i++;
        } else {
            script->unk00[0x11E] = arg;
            gRemoveEntityCommand(script, i);
        }
    }
}
