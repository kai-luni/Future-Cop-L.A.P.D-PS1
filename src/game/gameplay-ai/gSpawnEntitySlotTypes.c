// Categories: Gameplay > Map > Occupancy | Game Engine > Object Setup
#include "game/types.h"

extern int gGuidedProjectileUpdate();

struct Entity {
    u8 pad00[0x20];
    u8 column;
    u8 pad21[3];
    u32 flags;
};

struct Entry {
    u8 pad00[0x1A];
    u8 column;
};

extern struct Entry *gSlotSpawnDescs[] __asm__("D_800A0C70");
extern u8 gSelectEntitySlotType(u8 index, u8 slot, u8 mode) __asm__("func_8003B4E0");
extern s32 gGetEntitySpawnBank(struct Entity *entity) __asm__("func_8003BF50");
extern s32 gSpawnTypeAtOwner(struct Entity *entity, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");
void gSpawnEntitySlotTypes(u8 index, struct Entity *entity, s32 arg2, u16 flags, s32 mode) __asm__("func_8003B898");


/**
 * @brief Registers an entity's map occupancy in one or more grid columns.
 *
 * @param index Map region index into D_800A0C70.
 * @param entity Entity; +0x20 its column, +0x24 flags.
 * @param arg2 Forwarded to the occupancy writer.
 * @param flags 5-bit column mask for the multi-column mode.
 * @param mode 0 single entry column, 2 the special variant, else masked.
 * @return Nothing.
 *
 * @par Confirmed
 * Entity flag 0x40000 writes directly through func_800402DC with the
 * entity's own column and the row from func_8003BF50. Otherwise a non-null
 * D_800A0C70[index] entry is required: mode 0 uses the entry's column;
 * other modes resolve up to five columns via func_8003B4E0(index, i,
 * mode == 2) for each set mask bit, falling back to slot 0 when none hit.
 *
 * @par Likely role
 * The footprint writer called by spawn and tick paths (the base handler's
 * func_8002A810, the type-27 trigger, the player tick): large objects mark
 * several columns, the 0x40000 flag marks self-positioned entities.
 *
 * @par Unknown
 * What the special mode 2 changes in func_8003B4E0.
 *
 * @par Callers
 * func_8002A810, func_8002B1DC, func_8002ED7C, func_800324E0, gGuidedProjectileUpdate,
 * func_800382B4.
 *
 * @par Environment path
 * Spawns the definition types configured for an entity through
 * gSpawnTypeAtOwner (func_800402DC), always with bank func_8003BF50(entity):
 * with entity flag 0x40000 (+0x24) it spawns the single type in the entity's
 * byte +0x20; otherwise it requires the slot descriptor D_800A0C70[index] and
 * spawns its type byte +0x1A (mode 0) or, for every set bit of the 5-bit
 * mask, the type returned by func_8003B4E0(index, bit, mode == 2), falling
 * back to entry 0 when none was found. All six callers pass the entity's own
 * +0x20 slot and the mask (+0x19 >> 3) & 0x1E: gRetireEntityBase
 * (func_8002A810, 8002A810.c:117, mode 2), gGuidedProjectileUpdate
 * (80035B7C.c:298, when state +0xE0 is 4), gUpdateHoverEnemy
 * (800324E0.c:261), func_800382B4, func_8002B1DC and func_8002ED7C. It is the
 * same pattern as func_8003BAE4 and runs at runtime when such an entity is
 * destroyed or changes state; what the spawned types depict is not visible
 * here.
 */
void gSpawnEntitySlotTypes(u8 index, struct Entity *entity, s32 arg2, u16 flags, s32 mode)
{
    u8 i;
    u8 found;
    u8 column;
    u8 special;

    found = 0;
    special = mode == 2;
    if (entity->flags & 0x40000) {
        gSpawnTypeAtOwner(entity, entity->column, arg2, gGetEntitySpawnBank(entity), 0);
    } else if (gSlotSpawnDescs[index] != 0) {
        if (mode == 0) {
            if (gSlotSpawnDescs[index]->column != 0) {
                gSpawnTypeAtOwner(entity, gSlotSpawnDescs[index]->column, arg2,
                             gGetEntitySpawnBank(entity), 0);
            }
        } else {
            for (i = 0; i < 5; i++) {
                if (flags & 1) {
                    column = gSelectEntitySlotType(index, i, special);
                    if (column != 0) {
                        gSpawnTypeAtOwner(entity, column, arg2, gGetEntitySpawnBank(entity), 0);
                        found = 1;
                    }
                }
                flags >>= 1;
            }
            if (!found) {
                column = gSelectEntitySlotType(index, 0, special);
                if (column != 0) {
                    gSpawnTypeAtOwner(entity, column, arg2, gGetEntitySpawnBank(entity), 0);
                }
            }
        }
    }
}
