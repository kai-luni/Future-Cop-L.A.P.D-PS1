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
u8 gGetDestructionSpawnColumn(struct Unk3BObject *object) __asm__("func_8003B7D8");


/**
 * @brief Reads byte [1] from the entity's D_800A0C70 spawn-region table entry.
 *
 * Confirmed: returns 0 when object->tableIndex (+0x20) is 0, object->flags (+0x24) has bit 0x40000 set, or D_800A0C70[tableIndex] is null; otherwise returns entry[1]. Used by func_8002C174 as the spawn column passed to func_800402DC (defaulting 0 to 0x5B).
 *
 * @param object Entity block reading tableIndex at +0x20 and flags at +0x24.
 * @return Secondary spawn column byte entry[1], or 0 if masked or missing.
 *
 * @par Globals
 * D_800A0C70 - 256-entry region/spawn descriptor pointer table (read).
 *
 * @par Control flow
 * Three sequential early-return guards (tableIndex == 0, flags & 0x40000, entry == 0) before returning entry[1].
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8002C174 (0x8002C228, 0x8002C350 in asm/us/main.s) / Likely: none / Unknown: none.
 */
u8 gGetDestructionSpawnColumn(struct Unk3BObject *object)
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
    return entry[1];
}
