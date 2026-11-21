// Categories: Game Engine > Object Setup | Gameplay > Map | Graphics > Scene List
#include "game/types.h"

struct Unk3C500Cell {
    u8 unk00[0x14];
    u8 id;
    u8 next;
};

extern struct Unk3C500Cell *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern void gOrGridCellFlags(u8 column, s32 row, u8 bits) __asm__("func_8003C4C4");

/**
 * @brief Marks bit 2 (0x4) along a linked chain of D_800A7040 cell nodes until a stop node id is reached.
 *
 * Confirmed: selects row D_800A7040[row] (256-entry stride) and loops while cell->id (+0x14) != (u8)stop, reading cell = line[(u8)columnValue], advancing columnValue = cell->next (+0x15), and calling func_8003C4C4(columnValue, row, 4).
 * Category follows D_800A7040[row][256] cell descriptor array and D_800A6578 cell flags updated via func_8003C4C4/func_8003B3D8 during scene list cleanup in func_8003C590/func_8003FAD4.
 * Likely: propagates the flag bit across every node in a detected cycle from chain head column to tail stop; Unknown: semantic meaning of flag bit 0x4 in D_800A6578.
 *
 * @param column Starting 8-bit cell index within row of D_800A7040.
 * @param stop Stopping cell->id byte value; loop terminates after processing the node whose id matches (u8)stop.
 * @param row Row bank index into D_800A7040 and D_800A6578 (e.g., 0 or 1).
 *
 * @par Globals
 * D_800A7040 - 2D table of cell descriptor pointers indexed as [row][col]; D_800A6578 - flag grid mutated via func_8003C4C4.
 * @par Control flow
 * Do-while loop following cell->next (+0x15) links in D_800A7040[row] until cell->id == (u8)stop.
 * @par Calls
 * func_8003C4C4 - ORs bit 0x4 into the D_800A6578 byte returned by func_8003B3D8(columnValue, row).
 * @par Side effects
 * Sets bit 0x4 in D_800A6578 grid cells for every successor link along the traversed chain.
 * @par Callers
 * Confirmed: func_8003C590 (0x8003C754 in asm/us/main.s, when a cycle back-edge table[cell->next] == i is detected) / Likely: none / Unknown: none.
 */
void func_8003C500(s32 column, s32 stop, s32 row)
{
    struct Unk3C500Cell **line;
    struct Unk3C500Cell **base;
    struct Unk3C500Cell *cell;
    register s32 rowValue __asm__("$18");
    register s32 columnValue __asm__("$17");
    register s32 stopValue __asm__("$20");

    rowValue = row;
    columnValue = column;
    stopValue = stop;
    line = (base = gSpawnRecordMap[0], rowValue * 0x100 + base);
    do {
        cell = line[(u8)columnValue];
        __asm__("" : : "r"(columnValue));
        columnValue = cell->next;
        gOrGridCellFlags(columnValue, rowValue, 4);
    } while (cell->id != (u8)stopValue);
}
