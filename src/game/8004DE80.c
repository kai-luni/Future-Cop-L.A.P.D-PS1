// Categories: Gameplay > Entity Script | Gameplay > Entity Update | Game Engine > State Machine
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[8];
    u16 tag;
};

extern struct Entity *D_8009FA40;

/**
 * @brief Finds the first entity in D_8009FA40 matching a 16-bit tag and sends it message 0xC.
 *
 * Confirmed: traverses the active entity list D_8009FA40 (+0x00 next) comparing entity->tag (+0x14, u16) against tag, invokes entity->update(0xC, entity, arg, 0) (+0x04) on the first match and returns its result, or 0 if none matches. Invoked by script VM opcode 47 in func_80069D68.
 *
 * @param tag 16-bit entity tag compared against entity->tag (+0x14).
 * @param arg Script operand forwarded as arg2 to entity->update(0xC, entity, arg, 0).
 * @return Value returned by the matching entity's update callback, or 0 if not found.
 *
 * @par Globals
 * D_8009FA40 - head pointer of the active entity linked list (read-only walk).
 *
 * @par Control flow
 * Single linked-list scan along D_8009FA40->next with early indirect tail/call return on first tag match.
 *
 * @par Calls
 * Indirect call entity->update(0xC, entity, arg, 0) via function pointer at +0x04.
 *
 * @par Side effects
 * Executes the target entity's message 0xC handler; no direct global writes.
 *
 * @par Callers
 * Confirmed: func_80069D68 (0x8006A3EC in asm/us/main.s, script VM opcode 47) / Likely: none / Unknown: none.
 */
s32 func_8004DE80(s32 tag, s32 arg)
{
    struct Entity *entity;

    for (entity = D_8009FA40; entity != 0; entity = entity->next) {
        if (entity->tag == tag) {
            return entity->update(0xC, entity, arg, 0);
        }
    }
    return 0;
}
