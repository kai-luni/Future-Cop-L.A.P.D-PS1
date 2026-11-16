// Categories: Gameplay > Entity Update | Game Engine > Grid
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[0x0D];
    u8 unk19;
    u8 unk1A[2];
    s16 unk1C;
    u8 unk1E[6];
    u32 flags;
};

struct EntityTable {
    s32 count;
    s32 unk04;
    struct Entity *items[1];
};

extern struct EntityTable D_800A8F90;

extern void func_8003BAE4(struct Entity *entity, s32 value);

/**
 * @brief Runs per-frame deferred updates and grid re-placement across active entities in D_800A8F90.
 *
 * Scans D_800A8F90.items[0..count-1], skipping entries with id <= 0: entities with unk1C (+0x1C) < 0
 * receive message 4 via entity->update; otherwise entities with flags (+0x24) bit 0x4000 clear that bit
 * and call func_8003BAE4 using the 4-bit column mask extracted from unk19 (+0x19).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A8F90 - active entity table (count at +0x00, items array at +0x08).
 * @par Control flow
 * Counted for loop over D_800A8F90 with id <= 0 skip and mutually exclusive if/else-if on unk1C < 0 and (flags & 0x4000).
 * @par Calls
 * Indirect entity->update(4, entity, 0, 0) (+0x04) when unk1C < 0;
 * func_8003BAE4 - inserts entity into grid columns for mask (entity->unk19 >> 3) & 0x1E.
 * @par Side effects
 * Clears entity->flags bit 0x4000, updates grid occupancy via func_8003BAE4, and runs entity message 4 callbacks.
 * @par Callers
 * Confirmed: func_80027510 (two-player frame tick); also direct call at 0x80027388 in asm/us/main.s (single-player frame tick).
 */
void func_8004CE04(void)
{
    struct Entity *entity;
    s32 i;

    for (i = 0; i < D_800A8F90.count; i++) {
        entity = D_800A8F90.items[i];
        if (entity->id <= 0) {
            continue;
        }
        if (entity->unk1C < 0) {
            entity->update(4, entity, 0, 0);
        } else if (entity->flags & 0x4000) {
            entity->flags &= ~0x4000;
            func_8003BAE4(entity, (entity->unk19 >> 3) & 0x1E);
        }
    }
}
