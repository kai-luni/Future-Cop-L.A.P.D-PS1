// Categories: Gameplay > Object Spawning | Resources > Spawn Tables
#include "game/types.h"

struct Unk3B3F8 {
    u8 unk00[0x10];
    u8 flags;
};

extern void *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern u8 gSpawnTypeFlags[][0x100][4] __asm__("D_800A6578");

/**
 * @brief Copies spawn-table cell flags into a newly created object's +0x10 flags byte.
 *
 * Confirmed: masks column to 0..255, tests D_800A7040[row][column] != 0, and ORs D_800A6578[row][column][0] into object->flags (+0x10). Category follows spawn table D_800A7040 and map/spawn attribute grid D_800A6578.
 *
 * @param object Target object struct receiving flags at +0x10.
 * @param column Spawn-table column index, masked with 0xFF.
 * @param row Spawn-table row index (0 or 1 in func_8003E984).
 *
 * @par Globals
 * D_800A7040 - 256-column spawn-source pointer table (read); D_800A6578 - 256-column x 4-byte cell attribute table (byte [0] read).
 *
 * @par Control flow
 * Single conditional branch guarding object->flags |= D_800A6578[row][column][0] on D_800A7040[row][column] != 0.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Mutates object->flags (+0x10) when D_800A7040[row][column] is non-null.
 *
 * @par Callers
 * Confirmed: func_8003E984 (0x8003EBC0 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8003B3F8(struct Unk3B3F8 *object, u32 column, s32 row)
{
    column &= 0xFF;
    if (gSpawnRecordMap[row][column] != 0) {
        object->flags |= gSpawnTypeFlags[row][column][0];
    }
}
