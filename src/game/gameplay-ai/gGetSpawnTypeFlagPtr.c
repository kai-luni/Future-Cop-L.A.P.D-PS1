// Categories: Gameplay > Map
#include "../../include/game/types.h"

extern u32 gSpawnTypeFlags[] __asm__("D_800A6578");
u32 *gGetSpawnTypeFlagPtr(u32 column, u32 row) __asm__("func_8003B3D8");


/**
 * @brief Computes a pointer to a cell word in the 256-column map grid D_800A6578.
 *
 * Indexes the global u32 map cell table using a 1024-byte (256-entry) row stride and masks column to 8 bits (0..255).
 *
 * @param column Map column index (masked with 0xFF).
 * @param row Map row index (multiplied by 1024 bytes / 256 words).
 * @return Pointer to the u32 cell entry in D_800A6578.
 *
 * @par Globals
 * D_800A6578 - base address of the 256-column u32 map cell grid.
 *
 * @par Control flow
 * Straight-line address calculation: (u8 *)D_800A6578 + (row << 10) + ((column & 0xFF) << 2); no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8003C404 (0x8003C420), func_8003C4C4 (0x8003C4D4), func_8003C590 (0x8003C620), func_8003D680 (0x8003D914), func_8003E984 (0x8003EA48), and func_800402DC (0x8004032C).
 */
u32 *gGetSpawnTypeFlagPtr(u32 column, u32 row)
{
    u32 rowOffset;
    u32 columnOffset;

    rowOffset = row << 10;
    columnOffset = (column & 0xFF) << 2;
    return (u32 *)((u8 *)gSpawnTypeFlags + rowOffset + columnOffset);
}
