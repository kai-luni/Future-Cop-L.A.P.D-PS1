// Categories: Gameplay > Entity Script | AI > Attack Scheduling | Game Engine > Command Queue
#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Entry5FFB8 {
    u8 unk00[2];
    u16 unk02;
    u8 unk04[0x2C];
};

struct Obj5FFB8 {
    struct Entry5FFB8 *entry;
    u16 flags;
    u8 unk06;
    u8 pad07;
    u16 unk08;
    u8 pad0A[0x16];
    u16 unk20;
};

struct Script5FFB8 {
    u8 unk00[0x0C];
    struct Cmd commands[4];
};

extern struct Entry5FFB8 gEffectKeyTable[] __asm__("D_800A2300");
extern s32 gFindEffectKeyIndex(s32 key) __asm__("func_8005FF70");
extern void func_8002A5B4(struct Script5FFB8 *script, s32 index);
void gApplyAttackCommands(struct Obj5FFB8 *obj, struct Script5FFB8 *script) __asm__("func_8005FFB8");


/**
 * @brief Applies scripted attack-descriptor commands from an entity's 4-slot queue.
 *
 * Walks script->commands[0..3] (+0x0C), stopping on opcode 0. Handles opcodes
 * 0x1F..0x23 (target class/arg), 0x1E (enable/disable flag 0x100), and 0x26
 * (looks up entry in D_800A2300 by key and copies unk02 into obj->unk20).
 *
 * @param obj Attack descriptor (Obj5FFB8: +0x00 entry, +0x04 flags, +0x06/+0x08 class/arg, +0x20 gate).
 * @param script Host entity holding the 4-entry Command queue at +0x0C.
 *
 * @par Globals
 * D_800A2300 - attack definition table (0x30-byte Entry5FFB8 stride).
 *
 * @par Control flow
 * While i < 4, breaks on op == 0; handled opcodes call func_8002A5B4(script, i) without advancing i, while default opcodes advance p += 2 and i++.
 *
 * @par Calls
 * func_8005FF70 - finds index in D_800A2300 matching key byte; func_8002A5B4 - removes consumed command slot.
 *
 * @par Side effects
 * Updates obj->unk06, unk08, flags bit 0x100, entry, and unk20, and consumes matching slots from script->commands.
 *
 * @par Callers
 * Confirmed: func_80060238 (0x800602B4, 0x800602CC, merges commands from optional second and primary owner objects).
 */
void gApplyAttackCommands(struct Obj5FFB8 *obj, struct Script5FFB8 *script)
{
    register u8 *p __asm__("$17");
    s32 i;
    struct Entry5FFB8 *tbl;
    s32 op;
    s32 arg;
    s32 idx;

    i = 0;
    tbl = gEffectKeyTable;
    p = (u8 *)script;
    while (i < 4) {
        op = p[0xC];
        if (op == 0) {
            break;
        }
        arg = p[0xD];
        switch (op) {
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
            obj->unk06 = op - 0x1E;
            obj->unk08 = arg;
            break;
        case 0x1E:
            if (arg == 0) {
                obj->flags |= 0x100;
            } else {
                obj->flags &= 0xFEFF;
            }
            break;
        case 0x26:
            idx = gFindEffectKeyIndex(arg);
            if (idx >= 0) {
                obj->entry = (struct Entry5FFB8 *)(idx * 0x30 + (s32)tbl);
                obj->unk20 = obj->entry->unk02;
            }
            break;
        default:
            p += 2;
            i++;
            continue;
        }
        func_8002A5B4(script, i);
    }
}
