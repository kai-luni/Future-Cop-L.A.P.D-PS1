// Categories: Game Engine > Entity Dispatch
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[0x7C];
    s32 unk88;
    u8 unk8C[4];
    s16 unk90;
};

struct EntityTable {
    s32 count;
    s32 unk04;
    struct Entity *items[1];
};

extern struct EntityTable gCommandListHead __asm__("D_800A8F90");
void gDispatchEntityMsg0D(void) __asm__("func_8004EC74");


/**
 *  @brief Resets marked entities after sending them update message 0xD.
 *
 *  Scans the global entity table for entries with a positive ID and nonzero
 *  unk88, dispatching their update callback before resetting two fields.
 *  @par Globals
 *  D_800A8F90 supplies the entity count and pointer array.
 *  @par Control flow
 *  Iterates over the table and dispatches only when both eligibility tests pass.
 *  @par Calls
 *  Each eligible entity's update callback receives 0xD, the entity, zero, zero.
 *  @par Side effects
 *  Sets each eligible entity's unk88 to zero and unk90 to 0x7F00 after dispatch;
 *  callbacks may have additional effects.
 *  @par Callers
 *  Confirmed: func_80026D1C.
 */
void gDispatchEntityMsg0D(void)
{
    struct Entity **cursor;
    struct Entity *entity;
    s32 remaining;

    cursor = gCommandListHead.items;
    for (remaining = gCommandListHead.count; remaining > 0; remaining--) {
        entity = *cursor++;
        if (entity->id > 0 && entity->unk88 != 0) {
            entity->update(0xD, entity, 0, 0);
            entity->unk88 = 0;
            entity->unk90 = 0x7F00;
        }
    }
}
