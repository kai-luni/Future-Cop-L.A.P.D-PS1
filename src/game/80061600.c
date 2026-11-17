// Categories: Game Engine > Entity Dispatch
#include "game/types.h"

struct EntitySub2 {
    u8 unk00[0x1D];
    u8 unk1D;
};

struct EntitySub1 {
    u8 unk00[0x28];
    struct EntitySub2 *unk28;
};

struct Entity {
    void *unk00;
    struct EntitySub1 *unk04;
};

struct EntityTable {
    s32 count;
    struct Entity *items[1];
};

extern struct EntityTable *gRegistryTable __asm__("D_8009F800");

extern void func_800614BC(struct Entity *entity, s32 arg1, s32 arg2);

/**
 *  @brief Dispatches arguments to entities whose nested byte matches a message.
 *
 *  Walks the global entity table and compares each entity's nested unk1D byte
 *  with the supplied message before invoking the entity helper.
 *  @param message Value to compare with each entity's unk1D byte.
 *  @param arg1 First argument forwarded to the helper.
 *  @param arg2 Second argument forwarded to the helper.
 *  @par Globals
 *  D_8009F800 supplies the entity count and pointer array.
 *  @par Control flow
 *  Iterates over count entries and calls the helper only for matching entities.
 *  @par Calls
 *  func_800614BC receives each matching entity and the two forwarded arguments.
 *  @par Side effects
 *  The helper may modify matching entities or other state.
 *  @par Callers
 *  Confirmed: func_80069D68.
 */
void func_80061600(s32 message, s32 arg1, s32 arg2)
{
    struct Entity **cursor;
    struct Entity *entity;
    s32 remaining;

    cursor = gRegistryTable->items;
    for (remaining = gRegistryTable->count; remaining > 0; remaining--) {
        entity = *cursor++;
        if (entity->unk04->unk28->unk1D == message) {
            func_800614BC(entity, arg1, arg2);
        }
    }
}
