// Categories: Game Engine > Message Dispatch | Gameplay > Entity Update
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
};

struct EntityTable {
    s32 count;
    s32 unk04;
    struct Entity *items[1];
};

extern struct EntityTable gCommandListHead __asm__("D_800A8F90");

/**
 * @brief Broadcasts a message to every active entity in the global entity table D_800A8F90.
 *
 * Walks D_800A8F90.items[0..count-1] and invokes entity->update(message, entity, arg2, arg3)
 * (+0x04) for each entry whose id (+0x08) is strictly greater than 0.
 *
 * @param message Message id forwarded to each entity handler (6 during per-frame tick).
 * @param arg2 Second handler argument (current level id D_8009F9FC in frame ticks).
 * @param arg3 Third handler argument (0 in frame ticks).
 *
 * @par Globals
 * D_800A8F90 - active entity table (count at +0x00, items array at +0x08), read-only.
 * @par Control flow
 * Counted down-loop from D_800A8F90.count to 1 with an entity->id > 0 guard before the indirect call.
 * @par Calls
 * Indirect call through entity->update (+0x04) for each active entity.
 * @par Side effects
 * Any side effects are performed by the invoked entity update callbacks.
 * @par Callers
 * Confirmed: func_80027510 (two-player frame tick); also direct call at 0x80027374 in asm/us/main.s (single-player frame tick).
 */
void func_8004CD6C(s32 message, s32 arg2, s32 arg3)
{
    struct Entity **cursor;
    struct Entity *entity;
    s32 remaining;

    cursor = gCommandListHead.items;
    for (remaining = gCommandListHead.count; remaining > 0; remaining--) {
        entity = *cursor++;
        if (entity->id > 0) {
            entity->update(message, entity, arg2, arg3);
        }
    }
}
