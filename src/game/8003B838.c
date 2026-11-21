// Categories: Gameplay > Map | Resources > Spawn Tables
#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

extern u8 *gSlotSpawnDescs[] __asm__("D_800A0C70");
u8 gGetSpawnDescColumn(struct Unk3BObject *object) __asm__("func_8003B838");


/**
 * @brief Reads byte [0] from the entity's D_800A0C70 spawn-region table entry.
 *
 * Confirmed: returns 0 when object->tableIndex (+0x20) is 0, object->flags (+0x24) has bit 0x40000 set, or D_800A0C70[tableIndex] is null; otherwise returns entry[0]. Used by func_8003BC0C for map-occupancy registration via func_800402DC and by func_8003FA34 to gate deferred queueing.
 *
 * @param object Entity block reading tableIndex at +0x20 and flags at +0x24.
 * @return Primary spawn column byte entry[0], or 0 if masked or missing.
 *
 * @par Globals
 * D_800A0C70 - 256-entry region/spawn descriptor pointer table (read).
 *
 * @par Control flow
 * Three sequential early-return guards (tableIndex == 0, flags & 0x40000, entry == 0) before returning entry[0].
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8003BC0C (0x8003BC30 in asm/us/main.s), func_8003FA34 (0x8003FA78 in asm/us/main.s) / Likely: none / Unknown: none.
 */
u8 gGetSpawnDescColumn(struct Unk3BObject *object)
{
    u8 *entry;

    if (object->tableIndex == 0) {
        return 0;
    }
    if (object->flags & 0x40000) {
        return 0;
    }
    entry = gSlotSpawnDescs[object->tableIndex];
    if (entry == 0) {
        return 0;
    }
    return entry[0];
}
