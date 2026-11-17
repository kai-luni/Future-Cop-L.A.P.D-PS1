// Categories: Game Engine > Entity Management
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x0C];
    u8 team;
};

extern struct Entity *gUnitListHead __asm__("D_8009FA40");

extern u8 func_8004DA84(struct Entity *entity, s32 kind, s32 value, s32 retry);

/**
 *  @brief Applies an operation to each identified entity on a team.
 *
 *  Traverses the global entity list and forwards the operation parameters
 *  for entries whose team matches and whose ID is nonzero.
 *  @param team Team value to match against each entity.
 *  @param kind Operation kind forwarded to the helper.
 *  @param value Operation value forwarded to the helper.
 *  @par Globals
 *  D_8009FA40 is the head of the entity list.
 *  @par Control flow
 *  Walks next pointers to the end, skipping nonmatching or zero-ID entries.
 *  @par Calls
 *  func_8004DA84 receives each matching entity, kind, value, and retry 1;
 *  its return value is ignored.
 *  @par Side effects
 *  Invokes the helper for each match; its effects are not shown.
 *  @par Callers
 *  Confirmed: func_80069D68.
 */
void func_8004DDF4(s32 team, s32 kind, s32 value)
{
    struct Entity *entity;

    for (entity = gUnitListHead; entity != 0; entity = entity->next) {
        if (entity->team == team && entity->id != 0) {
            func_8004DA84(entity, kind, value, 1);
        }
    }
}
