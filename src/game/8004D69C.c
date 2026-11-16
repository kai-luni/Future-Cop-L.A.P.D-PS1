// Categories: Game Engine > Entity List | Gameplay > Entity Lookup
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[8];
    u16 tag;
};

extern struct Entity *D_8009FA40;

/**
 * @brief Searches the active entity linked list for the first entity matching a 16-bit tag.
 *
 * Walks the singly-linked list starting at D_8009FA40 using next (+0x00) until an entity whose
 * halfword tag (+0x14) equals the requested value is found or the list terminates.
 *
 * @param tag Target 16-bit entity tag value compared against entity->tag (+0x14).
 * @return Pointer to the matching struct Entity, or 0 if no entity has that tag.
 *
 * @par Globals
 * D_8009FA40 - head pointer of the active entity linked list.
 *
 * @par Control flow
 * Single pointer-chasing for-loop breaking early on entity->tag == tag.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_800313F0 (0x80031414, resolves attachment target from self->unkAC) / Likely: none / Unknown: none.
 */
struct Entity *func_8004D69C(s32 tag)
{
    struct Entity *entity;

    for (entity = D_8009FA40; entity != 0; entity = entity->next) {
        if (entity->tag == tag) {
            break;
        }
    }
    return entity;
}
