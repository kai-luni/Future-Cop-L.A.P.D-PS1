// Categories: Gameplay > Spawn Placement | Game Engine > Grid
#include "game/types.h"

struct Entity {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
};

extern u8 gCellAbortFlag __asm__("D_8009F31C");

extern void func_8003B6A4(s32 arg_);
extern s32 func_8003B65C(s32 value, s32 range);
extern u8 func_8003B5AC(u8 arg0, u8 arg1, s32 arg2);
extern s32 func_8003BF50(struct Entity *entity);
extern s32 func_800402DC(struct Entity *owner, u8 column, s32 arg2, s32 row, s32 arg4);

/**
 * @brief Places an entity into the grid columns selected by its flag word.
 *
 * func_8003B6A4 is called first (registration side effect on the entity).
 * The grid type comes from func_8003B65C on the cell coordinates 0x1C/0x1E;
 * type 3 means "no cell" and aborts. The global D_8009F31C is raised while the
 * placement runs. For each of the five low flag bits one column is resolved
 * with func_8003B5AC and, when found, func_800402DC is called with the owner,
 * that column, row 0, the result of func_8003BF50 and 0. If no bit matched, a
 * single placement with column selector 0 is attempted instead.
 *
 * @param entity Entity with cell coordinates at 0x1C/0x1E and selector at 0x20.
 * @param flags Bit mask, one bit per attempted column.
 *
 * @par Globals
 * D_8009F31C - "placing" flag read by the placement helpers.
 *
 * @par Control flow
 * Loop over five bits, then a fallback placement.
 *
 * @par Calls
 * func_8003B6A4 - entity registration. func_8003B65C - cell type lookup.
 * func_8003B5AC - column resolution. func_8003BF50 - row computation.
 * func_800402DC - actual placement.
 *
 * @par Side effects
 * Registers the entity and inserts it into grid columns.
 *
 * @par Callers
 * Likely: func_8004CE04 (same block, undocumented).
 */
void func_8003BAE4(struct Entity *entity, u16 flags)
{
    u8 i;
    u8 found;
    u8 column;
    s32 type;

    func_8003B6A4((s32)entity);

    found = 0;
    type = func_8003B65C(entity->unk1C, entity->unk1E);
    if (type == 3) {
        return;
    }

    gCellAbortFlag = 1;

    for (i = 0; i < 5; i++) {
        if (flags & 1) {
            column = func_8003B5AC(entity->unk20, i, type);
            if (column != 0) {
                func_800402DC(entity, column, 0, func_8003BF50(entity), 0);
                found = 1;
            }
        }
        flags >>= 1;
    }

    if (!found) {
        column = func_8003B5AC(entity->unk20, 0, type);
        if (column != 0) {
            func_800402DC(entity, column, 0, func_8003BF50(entity), 0);
        }
    }

    gCellAbortFlag = 0;
}
