// Categories: Game Engine > Entity Lifecycle | Game Engine > Object Pool
#include "game/types.h"

struct Entity {
    struct Entity *next;
};

extern struct Entity *D_8009FA40;
extern struct Entity *D_8009FA88;

/**
 * @brief Appends an entity node to the tail of the active singly linked entity list.
 *
 * Links entity onto the list rooted at D_8009FA40 (head) and D_8009FA88 (tail): attaches behind D_8009FA88 if non-empty or sets D_8009FA40 if empty, updates D_8009FA88 = entity, and zeroes entity->next.
 *
 * @param entity Entity block whose +0x00 word is its intrusive next pointer.
 *
 * @par Globals
 * D_8009FA40 - active entity list head (written when empty); D_8009FA88 - active entity list tail (always set to entity).
 *
 * @par Control flow
 * Single branch on D_8009FA88 != 0 selecting tail-append vs head-init, followed by tail and next updates.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Updates D_8009FA88->next or D_8009FA40, advances D_8009FA88, and clears entity->next.
 *
 * @par Callers
 * Confirmed: func_8004D3FC (0x8004D424), func_8004D444 (0x8004D4B0), func_8004D4F8 (0x8004D564).
 */
void func_8004D3D4(struct Entity *entity)
{
    if (D_8009FA88 != 0) {
        D_8009FA88->next = entity;
    } else {
        D_8009FA40 = entity;
    }
    D_8009FA88 = entity;
    entity->next = 0;
}
