// Categories: Game Engine > Object Lookup | Gameplay > Entity Lifecycle
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
};

extern struct Entity *D_8009FA40;

/**
 * @brief Searches the active entity linked list for an entry matching a given id.
 *
 * Linearly walks the singly-linked entity list rooted at D_8009FA40 until an entity whose +0x08 id field equals the requested id is found.
 *
 * @param id Entity identifier to look up (callers typically mask upper flag bits with 0x3FFFFFFF).
 * @return Pointer to the matching Entity struct, or 0 if not found.
 *
 * @par Globals
 * D_8009FA40 - head pointer of the active entity linked list; read at loop entry.
 *
 * @par Control flow
 * Single for-loop starting at D_8009FA40, advancing via entity->next (+0x00) until null or entity->id (+0x08) == id.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8002A810 (0x8002A86C), func_8002C174 (0x8002C19C), func_8002D664 (0x8002D928), func_800324E0 (0x8003298C), func_80058378 (0x800583F0), and func_80058D78 (0x80058F54).
 */
struct Entity *func_8004D664(s32 id)
{
    struct Entity *entity;

    for (entity = D_8009FA40; entity != 0; entity = entity->next) {
        if (entity->id == id) {
            break;
        }
    }
    return entity;
}
