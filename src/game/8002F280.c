// Categories: Gameplay > Entity Update | Game Engine > Command Queue
#include "game/types.h"

extern void func_8002A5B4(u8 *arg, s32 index);

/**
 * @brief Drains opcode 0x50 entries from a mover's 4-slot command queue (+0x0C..+0x13) into byte +0xA1.
 *
 * Scans the four `{cmd, param}` pairs at `arg + 0x0C..+0x13` until `cmd == 0` or slot 4: whenever `cmd == 0x50`, writes `param` into `arg[0xA1]` (`obj->direction` in func_8002F30C) and removes slot `i` via func_8002A5B4 without incrementing `i`. Category Gameplay > Entity Update | Game Engine > Command Queue follows queue compaction and caller func_8002F30C.
 *
 * @param arg Mover object byte pointer carrying four 2-byte command pairs at `+0x0C..+0x13` and direction at `+0xA1`.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * `while (i < 4)` breaking on `cmd == 0`; `cmd == 0x50` writes `arg[0xA1]` and compacts via func_8002A5B4, others increment `i`.
 *
 * @par Calls
 * func_8002A5B4 - removes slot `i` by shifting later command pairs down and clearing slot 3's opcode.
 *
 * @par Side effects
 * Writes `arg[0xA1]` and compacts handled `0x50` commands out of `arg + 0x0C..+0x13`.
 *
 * @par Callers
 * Confirmed: func_8002F30C (0x8002F334 in asm/us/main.s, when `obj->command != 0`) / Likely: none / Unknown: none.
 */
void func_8002F280(u8 *arg)
{
    s32 i;
    u8 cmd;
    u8 param;

    i = 0;
    while (i < 4) {
        cmd = arg[i * 2 + 0xC];
        if (cmd == 0) {
            break;
        }
        param = arg[i * 2 + 0xD];
        if (cmd == 0x50) {
            arg[0xA1] = param;
            func_8002A5B4(arg, i);
        } else {
            i++;
        }
    }
}
