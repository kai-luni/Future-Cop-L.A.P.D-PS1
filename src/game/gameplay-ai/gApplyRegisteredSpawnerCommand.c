// Categories: Gameplay > State
#include "game/types.h"

struct Unk614BCSub {
    u8 unk00[0xE];
    s16 unk0E;
    u16 flags;
    s16 unk12;
    u16 unk14;
    s16 unk16;
};

struct Unk614BCObj {
    u8 unk00[4];
    struct Unk614BCSub *sub;
};
void gApplyRegisteredSpawnerCommand(struct Unk614BCObj *obj, s32 arg1, s32 arg2) __asm__("func_800614BC");


/**
 *  @brief Updates flag fields in an object's substructure for two command codes.
 *
 *  Command 0x46 controls bit zero and an associated value; command 0x47
 *  increments a flag field only when the substructure passes two checks.
 *  @param obj Object containing the substructure to update.
 *  @param arg1 Command code, either 0x46 or 0x47.
 *  @param arg2 Selects the command's nonzero or zero branch.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Dispatches on arg1; 0x47 returns if unk16 is at least unk0E or unk12 is zero.
 *  Its nonzero branch increments bits 8 through 11; its zero branch adds 0x1000.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May change substructure flags and clear unk14 when clearing bit zero.
 *  @par Callers
 *  Confirmed: func_8006158C.
 */
void gApplyRegisteredSpawnerCommand(struct Unk614BCObj *obj, s32 arg1, s32 arg2)
{
    register struct Unk614BCSub *sub __asm__("$4") = obj->sub;
    u16 f;
    u16 v;

    switch (arg1) {
    case 0x46:
        if (arg2) {
            if (sub->flags & 1) {
                sub->flags &= 0xFFFE;
                sub->unk14 = 0;
            }
            return;
        }
        f = sub->flags | 1;
        break;
    case 0x47:
        if (sub->unk16 >= sub->unk0E) {
            return;
        }
        if (sub->unk12 == 0) {
            return;
        }
        if (arg2) {
            register u32 raw __asm__("$3");
            register u32 part __asm__("$2");
            raw = sub->flags;
            __asm__ volatile ("" : "+r" (raw));
            part = ((raw >> 8) & 0xF) + 1;
            __asm__ volatile ("" : "+r" (raw), "+r" (part));
            raw = (raw & 0xF0FF) | (part << 8);
            sub->flags = raw;
            return;
        }
        {
            register u32 raw __asm__("$2");
            register u32 high __asm__("$3");
            raw = sub->flags;
            __asm__ volatile ("" : "+r" (raw));
            high = raw & 0xF000;
            raw &= 0xFFF;
            __asm__ volatile ("" : "+r" (raw), "+r" (high));
            high += 0x1000;
            f = raw | high;
        }
        break;
    default:
        return;
    }
    sub->flags = f;
}
