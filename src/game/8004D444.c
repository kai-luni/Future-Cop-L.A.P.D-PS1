// Categories: Game Engine > Object Setup > Spawn Queue | Gameplay > Object Spawning
#include "game/types.h"

extern int gSpawnLobShot();
extern int gSpawnDirectShot();

struct Entity;

struct EntityDef {
    s32 (*handler)(s32 message, struct Entity *entity, struct EntityDef *def, s32 arg3);
};

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, struct EntityDef *def, s32 arg3);
    s32 id;
};

extern s32 gEntityIdCounter __asm__("D_8009F7A0");

extern void *func_80017EBC(s32 size, s32 tag);
extern void gAppendEntityToList(struct Entity *entity) __asm__("func_8004D3D4");

/**
 * @brief The spawn-queue core: allocates an entity from its handler's size
 *        query, assigns a fresh id, runs the spawn case and links it in.
 *
 * @param def Spawn definition; +0 the handler function pointer.
 * @return The new entity.
 *
 * @par Confirmed
 * handler(1, 0, 0, 0) is the size query (message 1 returns the block size);
 * func_80017EBC(size, 3) allocates from pool tag 3. The entity stores the
 * handler at +4 and id = ++D_8009F7A0 at +8; handler(2, entity, def, 0) runs
 * the spawn case, func_8004D3D4 links the entity into the active list.
 *
 * @par Likely role
 * The deferred spawn path behind every request spawner (func_80057914,
 * func_80039418, func_80058C3C and the effect spawners): the message-1/
 * message-2 protocol is the shared entity handler contract, the monotonic
 * id is what the pool GC uses to detect recycled objects.
 *
 * @par Callers
 * func_8002C584, func_80039418, func_8004D4D4, func_80057914, func_80058C3C,
 * gSpawnDirectShot, gSpawnLobShot, func_8005EB74, func_8005ECB8, func_8005EE04.
 */
struct Entity *gSpawnQueuedEntity(struct EntityDef *def) __asm__("func_8004D444");

struct Entity *gSpawnQueuedEntity(struct EntityDef *def)
{
    struct Entity *entity;
    s32 (*handler)(s32, struct Entity *, struct EntityDef *, s32);

    entity = func_80017EBC(def->handler(1, 0, 0, 0), 3);
    handler = def->handler;
    entity->update = handler;
    entity->id = ++gEntityIdCounter;
    handler(2, entity, def, 0);
    gAppendEntityToList(entity);
    return entity;
}
