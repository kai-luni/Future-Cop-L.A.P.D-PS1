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

extern u8 gGetSpawnDescColumn(struct Unk3BObject *object) __asm__("func_8003B838");
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
 *
 * @par Map-object path
 * Step 4 of the map-object path (docs/precinct-assault-map-objects.md): this
 * is the only place a map-occupancy write is actually performed. Both entry
 * points land here - immediately once the level is built (screen state 4 or
 * 0xD) or the deferred pool is gone (func_8003FA34), otherwise by replay
 * from func_8003FAD4. The unit contains no deferral logic itself, which is why
 * tests/host/map_objects_test.sh can observe func_8003FA34 purely through the
 * func_800402DC recorder: every "immediate" case in that suite goes through
 * this unit and its two callees.
 */
void gSpawnEntityPrimaryType(struct Unk3BObject *object)
{
    u8 kind;

    if (object->tableIndex != 0) {
        kind = gGetSpawnDescColumn(object);
        if (kind != 0) {
            gSpawnTypeAtOwner(object, kind, 0, gGetEntitySpawnBank(object), 0);
        }
    }
}
