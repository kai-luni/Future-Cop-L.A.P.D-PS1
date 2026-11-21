// Categories: Game Engine > Collision Grid
#include "game/types.h"

extern u32 *func_8003B3D8(u32 column, u32 row);

/**
 * @brief Sets flag bits on one grid cell.
 *
 * The cell record for the given column and row is resolved with func_8003B3D8
 * and its first byte receives the flags with a plain OR, so earlier bits
 * survive. No bounds check - a bad column/row pair is the caller's problem.
 *
 * @param column Column index inside the row.
 * @param row Row index of the grid.
 * @param bits Flags to add to the cell.
 *
 * @par Globals
 * None directly; the grid is reached through func_8003B3D8.
 *
 * @par Control flow
 * None.
 *
 * @par Calls
 * func_8003B3D8 - cell lookup.
 *
 * @par Side effects
 * Sets bits in the cell flag byte.
 *
 * @par Callers
 * Confirmed: func_8003C500, func_8003C590.
 */
void gOrGridCellFlags(u8 column, s32 row, u8 bits) __asm__("func_8003C4C4");
void gOrGridCellFlags(u8 column, s32 row, u8 bits)
{
    *(u8 *)func_8003B3D8(column, row) |= bits;
}
