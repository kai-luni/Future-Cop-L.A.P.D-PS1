// Categories: Game Engine > Object Setup > Spawn Queue | Gameplay > Object Spawning
#include "game/types.h"

struct Entity;

struct EntityDef {
    s32 (*handler)(s32 message, struct Entity *entity, struct EntityDef *def, s32 arg3);
};

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, struct EntityDef *def, s32 arg3);
    s32 id;
    u8 unk0C[0x18];
    u32 flags;
};

extern s32 gEntityIdCounter __asm__("D_8009F7A0");

extern void *func_80017EBC(s32 size, s32 tag);
extern void gAppendEntityToList(struct Entity *entity) __asm__("func_8004D3D4");

/**
 * @brief Allocates, initializes, links, and target-flags a new entity from an EntityDef descriptor.
 *
 * Queries block size via def->handler(1, ...), allocates from pool tag 3, assigns a monotonic ID from D_8009F7A0, runs spawn message 2, appends to the active list, and sets entity->flags bit 0x10000000. Category Game Engine > Object Setup > Spawn Queue | Gameplay > Object Spawning follows the entity pool allocation, handler handshake, and list linking.
 *
 * @param def Pointer to EntityDef whose first word is the entity message handler.
 * @return Newly assigned entity ID (entity->id).
 *
 * @par Globals
 * D_8009F7A0 - monotonic entity ID counter incremented on each spawn.
 *
 * @par Control flow
 * Straight-line sequence: size query (msg 1), allocation, handler/id init, spawn call (msg 2), list append, flag set, return.
 *
 * @par Calls
 * def->handler - indirect msg 1 (size) and msg 2 (spawn); func_80017EBC - pool tag 3 alloc; func_8004D3D4 - append to D_8009FA40/D_8009FA88.
 *
 * @par Side effects
 * Increments D_8009F7A0, allocates entity memory, appends to the active entity list, and sets entity->flags |= 0x10000000.
 *
 * @par Callers
 * Confirmed: func_8006125C (0x8006148C in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 gSpawnTargetableEntityId(struct EntityDef *def) __asm__("func_8004D4F8");

s32 gSpawnTargetableEntityId(struct EntityDef *def)
{
    struct Entity *entity;
    s32 (*handler)(s32, struct Entity *, struct EntityDef *, s32);

    entity = func_80017EBC(def->handler(1, 0, 0, 0), 3);
    handler = def->handler;
    entity->update = handler;
    entity->id = ++gEntityIdCounter;
    handler(2, entity, def, 0);
    gAppendEntityToList(entity);
    entity->flags |= 0x10000000;
    return entity->id;
}
