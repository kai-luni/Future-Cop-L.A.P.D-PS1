// Categories: Gameplay > Map > Occupancy | Game Engine > Object Setup
#include "game/types.h"

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

extern struct Entry *gOccupancyColumns[] __asm__("D_800A0C70");
extern u8 func_8003B4E0(u8 index, u8 slot, u8 mode);
extern s32 func_8003BF50(struct Entity *entity);
extern s32 func_800402DC(struct Entity *entity, u8 column, s32 arg2, s32 row, s32 arg4);

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
 * func_8002A810, func_8002B1DC, func_8002ED7C, func_800324E0, func_80035B7C,
 * func_800382B4.
 */
void func_8003B898(u8 index, struct Entity *entity, s32 arg2, u16 flags, s32 mode)
{
    u8 i;
    u8 found;
    u8 column;
    u8 special;

    found = 0;
    special = mode == 2;
    if (entity->flags & 0x40000) {
        func_800402DC(entity, entity->column, arg2, func_8003BF50(entity), 0);
    } else if (gOccupancyColumns[index] != 0) {
        if (mode == 0) {
            if (gOccupancyColumns[index]->column != 0) {
                func_800402DC(entity, gOccupancyColumns[index]->column, arg2,
                             func_8003BF50(entity), 0);
            }
        } else {
            for (i = 0; i < 5; i++) {
                if (flags & 1) {
                    column = func_8003B4E0(index, i, special);
                    if (column != 0) {
                        func_800402DC(entity, column, arg2, func_8003BF50(entity), 0);
                        found = 1;
                    }
                }
                flags >>= 1;
            }
            if (!found) {
                column = func_8003B4E0(index, 0, special);
                if (column != 0) {
                    func_800402DC(entity, column, arg2, func_8003BF50(entity), 0);
                }
            }
        }
    }
}
