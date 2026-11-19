// Categories: Gameplay > Object Spawning | Gameplay > Map | Game Engine > Object Setup
#include "game/types.h"

extern int gWriteCellEventValue();
extern int gAttachPartEffect();
extern int gGuidedProjectileUpdate();
extern int gBeamPartUpdate();
extern int gWeaponFireDispatch();

struct Unk402DCOwner {
    u8 unk00[8];
    s32 unk08;
};

extern void *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern u8 gBatchSpawnActive __asm__("D_8009F31C");

extern u32 *func_8003B3D8(u32 column, u32 row);
extern void *gCreateModelNode(struct Unk402DCOwner *owner, u8 column, s32 row, s32 arg3,
                           s32 arg4, u8 arg5) __asm__("func_8003D680");
extern s32 gSpawnObjectFromTable(void *object, s32 arg1, u8 column, s32 arg3, s32 row) __asm__("func_8003E984");
extern void *func_8003FD74(s32 column, s32 row, struct Unk402DCOwner *owner, s32 arg3);
s32 gSpawnTypeAtOwner(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");


/**
 * @brief Map-cell spawn gate: spawns an object into a cell unless the cell's
 *        flags demand the deferred owner path.
 *
 * @param owner Optional driver block; its +0x08 word selects the deferred path.
 * @param column Cell column (low byte used by the map accessor).
 * @param arg2 Forwarded to the object constructor and the spawn call.
 * @param row Cell row.
 * @param arg4 Forwarded to the object constructor.
 * @return The spawn result of func_8003E984, or 0 when the cell is empty,
 *         the deferred path was taken, or no owner was given.
 *
 * @par Confirmed
 * D_800A7040[row][column] must be non-zero or the call returns 0. The cell
 * word comes from func_8003B3D8(column, row) (map base D_800A6578 +
 * row << 10 + (column & 0xFF) << 2). When (word & 5) == 4 the cell takes the
 * special path: with a non-null owner whose +0x08 is non-zero it queues a
 * 0x20-byte request node through func_8003FD74(column, row, owner, 0) and
 * returns 0; without an owner it also returns 0. Otherwise it builds the
 * object with func_8003D680(owner, column, row, arg2, arg4, D_8009F31C)
 * (a 0x4C-byte pool allocation) and registers it via func_8003E984.
 *
 * @par Likely role
 * The shared entry point of the entity spawn handlers: the flag pair 5 == 4
 * marks cells that must not spawn directly (occupied or scripted), so the
 * request is parked for later processing instead.
 *
 * @par Unknown
 * The exact meaning of cell flag bits 0 and 2, and what D_8009F31C selects
 * in the constructor.
 *
 * @par Callers
 * func_8002C174, func_800306A0, gGuidedProjectileUpdate, func_80036BD4, func_8003B898,
 * func_8003BAE4, func_8003BC0C, func_8003FD10, func_8004C0C0, func_800584BC,
 * func_800588D4, func_80058D78, func_8005B6A8, func_8005BDE4, gWriteCellEventValue,
 * func_8005C184, func_8005C344, gAttachPartEffect, func_8005F018, func_8005F6E8,
 * gWeaponFireDispatch, gBeamPartUpdate.
 *
 * @par Environment path
 * Common entry for spawning an object of definition type 'column' from bank
 * 'row' next to an owner: it returns 0 unless gSpawnRecordMap[row][column]
 * (D_800A7040) holds a definition; when the type's flag word from
 * func_8003B3D8 has (word & 5) == 4 it only parks a request via func_8003FD74
 * (if owner and owner+0x08 are set) and returns 0; otherwise it creates a
 * 0x4C-byte anchor node with func_8003D680, passing gBatchSpawnActive
 * (D_8009F31C) as its override flag, and hands it to func_8003E984, which
 * builds and registers the object. The 'column/row' parameter names in the
 * source are really type index and bank, as 8003C404.c and 8003E984.c index
 * the same table. It is called from 22 runtime paths
 * (gGuidedProjectileUpdate, gAttachPartEffect, gWeaponFireDispatch,
 * gBeamPartUpdate, func_8003BAE4 and others) to spawn effects and attached
 * objects during play.
 */
s32 gSpawnTypeAtOwner(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4)
{
    if (gSpawnRecordMap[row][column] == 0) {
        return 0;
    }
    if ((*(u16 *)func_8003B3D8(column, row) & 5) == 4) {
        if (owner == 0) {
            return 0;
        }
        if (owner->unk08 != 0) {
            func_8003FD74(column, row, owner, 0);
        }
        return 0;
    }
    return gSpawnObjectFromTable(gCreateModelNode(owner, column, row, arg2, arg4, gBatchSpawnActive), 0,
                         column, arg2, row);
}
