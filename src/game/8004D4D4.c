// Categories: Game Engine > Object Setup > Spawn Queue | Gameplay > Object Spawning
#include "game/types.h"

struct EntityDef;

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
};

extern struct Entity *gSpawnQueuedEntity(struct EntityDef *def) __asm__("func_8004D444");
s32 gSpawnQueuedEntityAndGetId(struct EntityDef *def) __asm__("func_8004D4D4");


/**
 * @brief Spawns an entity from a request record and returns only its new id.
 *
 * gSpawnQueuedEntity runs the whole handshake - handler(1, ...) for the block size, allocation from pool tag 3, id = ++D_8009F7A0, handler(2, ...) for the spawn case, then linking into the active list - and hands the entity back; this wrapper drops the pointer and returns the id at +8.
 *
 * @param def Spawn definition; word +0 is the handler function pointer.
 * @return The id assigned to the new entity.
 *
 * @par Globals
 * None directly; the id counter D_8009F7A0 is bumped inside gSpawnQueuedEntity.
 *
 * @par Control flow
 * One call, one field load at +8, return.
 *
 * @par Calls
 * gSpawnQueuedEntity - allocates, initialises and links the entity.
 *
 * @par Side effects
 * A new entity exists in the active list and one pool block is consumed, even though the caller only sees an integer.
 *
 * @par Callers
 * Confirmed: func_80031690 (0x800317A0) and func_8003A8C0 (0x8003A8F4) queue their spawn requests through it.
 * Unknown: one call at 0x8004D8F0 from a routine that is not in the matching set.
 */
s32 gSpawnQueuedEntityAndGetId(struct EntityDef *def)
{
    return gSpawnQueuedEntity(def)->id;
}
