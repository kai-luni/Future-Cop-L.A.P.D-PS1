// Categories: Gameplay > Map > Occupancy
#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

extern u8 func_8003B838(struct Unk3BObject *object);
extern s32 gGetEntitySpawnBank(struct Unk3BObject *object) __asm__("func_8003BF50");
extern void gSpawnTypeAtOwner(struct Unk3BObject *object, u8 kind, s32 arg2,
                          s32 arg3, s32 arg4) __asm__("func_800402DC");
void gSpawnEntityPrimaryType(struct Unk3BObject *object) __asm__("func_8003BC0C");


/**
 * @brief Refreshes an entity's map occupancy through its resolved column
 *        kind.
 *
 * @param object Entity; +0x20 the table index, +0x24 flags.
 * @return Nothing.
 *
 * @par Confirmed
 * Only when tableIndex != 0: kind = func_8003B838(object); a non-zero kind
 * writes occupancy via func_800402DC(object, kind, 0, func_8003BF50(object),
 * 0) (the row comes from the kind predicate).
 *
 * @par Likely role
 * The per-tick occupancy update of the mover handlers (func_800371FC,
 * func_8003FAD4): as the entity crosses cells the column kind is
 * re-resolved and the grid mark rewritten.
 *
 * @par Callers
 * func_800371FC, func_8003FAD4.
 */
void gSpawnEntityPrimaryType(struct Unk3BObject *object)
{
    u8 kind;

    if (object->tableIndex != 0) {
        kind = func_8003B838(object);
        if (kind != 0) {
            gSpawnTypeAtOwner(object, kind, 0, gGetEntitySpawnBank(object), 0);
        }
    }
}
