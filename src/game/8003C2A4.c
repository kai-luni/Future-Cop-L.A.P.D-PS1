// Categories: Gameplay > Object Spawning | Resources > Spawn Tables | Physics > Collision
#include "game/types.h"

struct Vec3C2A4 {
    s32 x;
    s32 y;
    s32 z;
};

struct UnkC2A4 {
    u8 unk00[8];
    struct Vec3C2A4 position;
    u8 unk14;
    u8 unk15[3];
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u8 unk1E[0x14];
    u16 flags;
};

struct OutC2A4 {
    u8 unk00[0x10];
    struct Vec3C2A4 position;
    u8 unk1C[0x2D];
    u8 unk49;
};

extern s32 gSavedChunkBase __asm__("D_8009F310");
extern struct UnkC2A4 *gSpawnRecordMap[][0x100] __asm__("D_800A7040");

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800188D0(s32 arg0, s32 arg1, s32 arg2, s32 *out);
extern s32 func_80066F78(struct Vec3C2A4 *position, s32 arg1, s32 arg2, s32 arg3);
extern void *func_8003FD74(s32 type, s32 arg08, void *source, s32 arg18);

/**
 * @brief Queues a deferred spawn request with rotated ground-offset placement when a spawn record has flag bit 0 set, and checks table occupancy.
 *
 * Confirmed: returns 0 if data is null or (data->flags & 1) == 0 (+0x32). Otherwise allocates 0x4C bytes via func_80017EBC(0x4C, 2), sets out->unk49 = 4, builds a 3x3 matrix from (unk18, -unk1A, 0) via func_800188D0, offsets data->position by (matrix[6..8] >> 4) * unk1C >> 8 with Y based on func_80066F78(&data->position, 0, 0, 0), and queues func_8003FD74(data->unk14, D_8009F310, 0, out). Returns 1 if D_800A7040[D_8009F310][data->unk14] is non-null with bit 0 of +0x32 set, else 0. Category follows D_800A7040 table, func_8003FD74 queue, func_800188D0 rotation, and func_80066F78 ground height.
 * Likely: pre-registration gate used by class-definition loaders to defer initial instance placement and prevent overwriting an existing deferred entry. Unknown: meaning of out->unk49 == 4 and why callers pass structs of varying size (0x58/0x60).
 *
 * @param data Spawn source descriptor (+0x08 position, +0x14 table column, +0x18/+0x1A angles, +0x1C forward offset, +0x32 flags).
 * @return 1 if D_800A7040[D_8009F310][data->unk14] already holds an entry with flags bit 0 set, 0 otherwise.
 *
 * @par Globals
 * D_8009F310 - active spawn-table row index (read). D_800A7040 - 2D spawn source pointer table (read at [D_8009F310][data->unk14]).
 * @par Control flow
 * Early exits on null data or clear flags bit 0; linear transform and deferred queue, then final table bit check.
 * @par Calls
 * func_80017EBC - allocates 0x4C-byte tag-2 placement record. func_800188D0 - builds rotation matrix. func_80066F78 - queries terrain Y height. func_8003FD74 - prepends deferred spawn node to D_8009F30C.
 * @par Side effects
 * Allocates a 0x4C-byte heap block and prepends a deferred spawn node to D_8009F30C via func_8003FD74.
 * @par Callers
 * Confirmed: func_80040DCC (0x80040DD8), func_80044770 (0x8004477C), func_8004C884 (0x8004C890), func_8006EAF0 (0x8006EAFC), func_800703A8 (0x800703B4), func_80073F88 (0x80073F94), func_800767E4 (0x800767F0) / Likely: none / Unknown: none.
 */
u8 func_8003C2A4(struct UnkC2A4 *data)
{
    struct OutC2A4 *out;
    struct UnkC2A4 *object;
    s32 matrix[9];

    if (data == 0) {
        return 0;
    }
    if ((data->flags & 1) == 0) {
        return 0;
    }
    out = func_80017EBC(0x4C, 2);
    out->unk49 = 4;
    func_800188D0(data->unk18, -data->unk1A, 0, matrix);
    out->position = data->position;
    out->position.x += ((matrix[6] >> 4) * data->unk1C) >> 8;
    out->position.y = func_80066F78(&data->position, 0, 0, 0) +
                      (((matrix[7] >> 4) * data->unk1C) >> 8);
    out->position.z += ((matrix[8] >> 4) * data->unk1C) >> 8;
    func_8003FD74(data->unk14, gSavedChunkBase, 0, (s32)out);
    object = gSpawnRecordMap[gSavedChunkBase][data->unk14];
    if (object != 0 && (object->flags & 1) != 0) {
        return 1;
    }
    return 0;
}
