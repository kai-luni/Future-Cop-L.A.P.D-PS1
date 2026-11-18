// Categories: Game Engine > Entity Lifecycle | Gameplay > Object Spawning
#include "game/types.h"

extern int gSpawnQueuedEntity();

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
};

extern s32 gEntityIdCounter __asm__("D_8009F7A0");

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_8004D3D4(struct Entity *entity);

/**
 * @brief Allocates an entity block of a given size, stamps a fresh serial id and
 *        links it into the active entity list.
 *
 * The slim variant of gSpawnQueuedEntity: the size comes from the caller instead of a
 * handler's size query and no spawn message is sent. The id at +0x8 comes from
 * the monotonic D_8009F7A0, which lets the pool garbage collector tell recycled
 * blocks from live ones; the block is then appended at the tail of the active
 * list through its next pointer at +0x0.
 *
 * @param size Block size in bytes, forwarded to the allocator unchanged (the
 *        debris burst of func_80036BD4 asks for 0xD4).
 * @return The new block. The allocator is assumed to succeed - there is no null
 *         check before the id store.
 *
 * @par Globals
 * D_8009F7A0 - entity id counter, pre-incremented per allocation and zeroed by
 * the world resets func_8004CAE4 and func_8004CCDC.
 *
 * @par Control flow
 * Linear: allocate, store the id, link, return. No branches.
 *
 * @par Calls
 * func_80017EBC - tagged allocator, tag 3 (entity pool).
 * func_8004D3D4 - append to the active list kept in D_8009FA40/D_8009FA88.
 *
 * @par Side effects
 * Allocates from the entity pool; writes the block's +0x0 and +0x8, the counter
 * and the list tail.
 *
 * @par Callers
 * Confirmed: func_80036BD4 (four debris children per burst, size 0xD4, each
 * copying a 0xD4-byte record and fixing up the two pool header words).
 */
struct Entity *func_8004D3FC(s32 size)
{
    struct Entity *entity;

    entity = func_80017EBC(size, 3);
    entity->id = ++gEntityIdCounter;
    func_8004D3D4(entity);
    return entity;
}
