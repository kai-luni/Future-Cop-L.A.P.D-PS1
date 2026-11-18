// Categories: Game Engine > Entity Lifecycle | Game Engine > World Reset
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[0x18];
    u32 flags;
};

extern struct Entity *gUnitListHead __asm__("D_8009FA40");
extern s32 gEntityIdCounter __asm__("D_8009F7A0");
extern void *gPlayerEntities[] __asm__("D_8009FA58");

extern void func_8004D958(struct Entity *entity);
extern void gSweepRetiredEntities(void) __asm__("func_8004D598");

/**
 * @brief Retires and frees all active entities and resets entity-world globals.
 *
 * Walks the active entity list, marks every active entity with flag 0x4000000 and
 * retires it, sweeps retired slots to free them, and clears ID and player globals.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA40 - active entity list head; D_8009F7A0 - ID counter cleared to 0;
 * D_8009FA58 - two-slot player pointer array cleared to null.
 *
 * @par Control flow
 * Walks D_8009FA40 retiring entities with id != 0, calls func_8004D598 to free
 * retired entries, then clears D_8009FA58[1..0] in a reverse 2-iteration loop.
 *
 * @par Calls
 * func_8004D958 - retires each active entity; func_8004D598 - unlinks and frees id==0 slots.
 *
 * @par Side effects
 * Sets flag 0x4000000 on active entities, frees all active entities, resets D_8009F7A0 and D_8009FA58[0..1].
 *
 * @par Callers
 * Confirmed: func_80024D14 (global subsystem reset).
 */
void gRetireAndFreeAllEntities(void) __asm__("func_8004CCDC");

void gRetireAndFreeAllEntities(void)
{
    struct Entity *entity;
    s32 i;

    for (entity = gUnitListHead; entity != 0; entity = entity->next) {
        if (entity->id != 0) {
            entity->flags |= 0x4000000;
            func_8004D958(entity);
        }
    }
    gSweepRetiredEntities();
    gEntityIdCounter = 0;
    for (i = 1; i >= 0; i--) {
        gPlayerEntities[i] = 0;
    }
}
