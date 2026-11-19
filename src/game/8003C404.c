// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk3C404 {
    u8 unk00[0x14];
    u8 unk14;
    u8 unk15[0x13];
    s16 unk28;
    s16 unk2A;
};

extern u32 gSpawnTableRow __asm__("D_8009F310");
extern void *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern u32 *func_8003B3D8(u32 column, u32 row);
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gInstallSpawnRecord(struct Unk3C404 *arg0) __asm__("func_8003C404");


/**
 * @brief Installs a spawn record into D_800A7040 and clears its map grid byte.
 *
 * Clears the map-grid byte returned by func_8003B3D8 for (arg0->unk14, D_8009F310),
 * frees any prior occupant at that D_800A7040 slot, normalizes unk28/unk2A, and stores arg0.
 *
 * @param arg0 Spawn record (+0x14 column index, +0x28/+0x2A s16 state fields).
 *
 * @par Globals
 * D_8009F310 - active spawn-table row index; D_800A7040 - 2D spawn-record pointer table.
 *
 * @par Control flow
 * Frees D_800A7040[D_8009F310][arg0->unk14] if non-null; clears unk2A when unk28 == 1; forces unk2A = 0 and unk28 = -1 when unk2A <= 0.
 *
 * @par Calls
 * func_8003B3D8 - locates cell pointer in D_800A6578; func_800181D4 - frees previously installed slot record.
 *
 * @par Side effects
 * Clears *(u8 *)cell, may mutate arg0->unk28/unk2A, frees prior entry, and writes D_800A7040[D_8009F310][arg0->unk14].
 *
 * @par Callers
 * Confirmed: func_80040DCC (0x80040E44), func_80044770 (0x800447D4), func_8004C884 (0x8004C904), func_8006EAF0 (0x8006EB64), func_800703A8 (0x80070420), func_80073F88 (0x80074008), func_800767E4 (0x8007685C).
 *
 * @par Environment path
 * Installs one copied actor record into the spawn table
 * D_800A7040[D_8009F310][record +0x14]: it first clears the low byte of the
 * parallel status word D_800A6578[row][id] returned by func_8003B3D8, frees
 * any record already in that slot, normalises the state half-words (+0x28 ==
 * 1 forces +0x2A to 0; +0x2A <= 0 sets +0x2A = 0 and +0x28 = -1) and stores
 * the pointer. Callers are the seven type registrars func_80040DCC,
 * func_80044770, func_8004C884, func_8006EAF0, func_800703A8, func_80073F88
 * and func_800767E4, reached from the 'Cact' loader func_8004D6D4 and the
 * 'Csfx' stream walker func_8003BFD0 during level loading. The table is read
 * later by the spawn code (func_8003C2A4, func_8003E984, func_800402DC,
 * gCreateModelNode func_8003D680 and others) to build live objects.
 */
void gInstallSpawnRecord(struct Unk3C404 *arg0)
{
    *(u8 *)func_8003B3D8(arg0->unk14, gSpawnTableRow) = 0;
    if (gSpawnRecordMap[gSpawnTableRow][arg0->unk14] != 0) {
        gFreeBlock(gSpawnRecordMap[gSpawnTableRow][arg0->unk14]);
    }
    if (arg0->unk28 == 1) {
        arg0->unk2A = 0;
    }
    if (arg0->unk2A <= 0) {
        arg0->unk2A = 0;
        arg0->unk28 = -1;
    }
    gSpawnRecordMap[gSpawnTableRow][arg0->unk14] = arg0;
}
