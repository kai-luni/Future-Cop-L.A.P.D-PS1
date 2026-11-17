// Categories: Game Engine > Entity Lifecycle | Game Engine > Object Pool
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[0x18];
    u32 flags;
};

extern struct Entity *gUnitListHead __asm__("D_8009FA40");
extern struct Entity *gListTailBranch __asm__("D_8009FA88");

extern void func_80061020(struct Entity *entity);
extern void func_800181D4(void *block);

/**
 * @brief Sweeps retired (id == 0) entities from the active list and frees them.
 *
 * Walks D_8009FA40; for every entity with id == 0, clears references via func_80061020 if
 * flag 0x10000000 is set, dispatches teardown message 5, unlinks it, updates tail D_8009FA88, and frees it.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA40 - active entity linked list head; D_8009FA88 - list tail pointer updated when the last node is removed.
 * @par Control flow
 * Single while loop over D_8009FA40 keeping previous; on id == 0 removes node (updating D_8009FA88 if new next is null) and frees it.
 * @par Calls
 * func_80061020 (target cleanup when flags & 0x10000000), entity->update(5, entity, 0, 0) (teardown), func_800181D4 (frees entity).
 * @par Side effects
 * Unlinks and frees dead entities, mutates D_8009FA40/D_8009FA88, and invokes entity teardown callbacks.
 * @par Callers
 * Confirmed: func_80027510 (main update), func_8004CCDC (entity world reset), func_800271A4 (at 0x8002749C in asm/us/main.s).
 */
void func_8004D598(void)
{
    struct Entity *entity;
    struct Entity *next;
    struct Entity *previous;

    previous = 0;
    next = gUnitListHead;
    while (next != 0) {
        entity = next;
        next = next->next;
        if (entity->id == 0) {
            if (entity->flags & 0x10000000) {
                func_80061020(entity);
            }
            entity->update(5, entity, 0, 0);
            if (previous != 0) {
                if ((previous->next = entity->next) != 0) {
                    goto release;
                }
            } else {
                if ((gUnitListHead = entity->next) != 0) {
                    goto release;
                }
            }
            gListTailBranch = previous;
release:
            func_800181D4(entity);
        } else {
            previous = entity;
        }
    }
}
