// Categories: Game Engine > Object Setup | Gameplay > Map | Graphics > Scene List
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct UnkC590Cell {
    u8 unk00[4];
    u8 type;
    u8 unk05[0x0F];
    u8 id;
    u8 next;
    u8 unk16[0x0A];
    s32 unk20;
    u8 unk24[4];
    s16 unk28;
    s16 unk2A;
    u8 unk2C[0x1C];
    s16 unk48;
} UnkC590Cell;

extern UnkC590Cell *gSpawnRecordMap[][0x100] __asm__("D_800A7040");
extern u32 *func_8003B3D8(u32 column, u32 row);
extern void gOrGridCellFlags(u8 column, s32 row, u8 bits) __asm__("func_8003C4C4");
extern void func_8003C500(s32 column, s32 stop, s32 row);

/**
 * @brief Traverses object-descriptor chains in one row of D_800A7040 and flags active or cyclic nodes in D_800A6578.
 *
 * Confirmed: zeroes a 256-entry local s16 table, scans D_800A7040[arg0][1..255], skips empty, visited, or already-flagged (bit 0x4 via func_8003B3D8) slots, follows cell->next (+0x15) chains while marking bit 0x4 via func_8003C4C4 when (unk28==0 && unk2A>0) or (unk20==0 && type in {0x57..0x59} or (type==0x5C && unk48==0)), and calls func_8003C500 on cycle closure.
 * Category follows D_800A7040/D_800A6578 spawn-descriptor tables used by func_8003E984 and invoked twice during teardown in func_8003FAD4.
 * Likely: propagates reachability/visibility flag bit 0x4 across descriptor chains and cycles for bank arg0 (0 or 1); Unknown: exact meaning of flag bit 0x4 in D_800A6578.
 *
 * @param arg0 Row bank index into D_800A7040 and D_800A6578 (0 or 1 in func_8003FAD4).
 *
 * @par Globals
 * D_800A7040 - 2D array of UnkC590Cell descriptor pointers (1024-byte row stride); D_800A6578 - flag grid read/updated via func_8003B3D8 and func_8003C4C4.
 * @par Control flow
 * Outer loop over i = 1..255 with inner chain_loop following cell->next (+0x15), a switch on cell->type (+0x04), and early cycle exit via func_8003C500 when table[cell->next] == i.
 * @par Calls
 * func_8003B3D8 - checks bit 0x4 in D_800A6578; func_8003C4C4 - sets bit 0x4 on qualifying nodes; func_8003C500 - marks bit 0x4 along a closed cycle from i to current.
 * @par Side effects
 * Sets bit 0x4 in D_800A6578 entries for qualifying descriptor chains and cycles in row arg0.
 * @par Callers
 * Confirmed: func_8003FAD4 (0x8003FAE4 with arg0=0, 0x8003FAEC with arg0=1 during screen teardown) / Likely: none / Unknown: none.
 */
void func_8003C590(s32 arg0)
{
    s16 table[256];
    register s32 row __asm__("$23");
    register s32 i __asm__("$18");
    register s32 current __asm__("$17");
    register UnkC590Cell *cell __asm__("$16");
    register s16 *tableBase __asm__("$19");
    register UnkC590Cell **line __asm__("$20");
    register UnkC590Cell **scan __asm__("$21");
    register s16 *visited __asm__("$22");
    register void *base __asm__("$2");
    register s32 rowOff __asm__("$3");
    s16 *ptr;
    u8 flag;

    row = arg0;
    for (i = 255, ptr = &table[255]; i >= 0; i--) {
        *ptr-- = 0;
    }

    i = 1;
    tableBase = table;
    visited = table + 1;
    base = gSpawnRecordMap[0];
    rowOff = row << 10;
    line = (UnkC590Cell **)(rowOff + (s32)base);
    scan = line + 1;

outer_loop:
    cell = *scan;
    if (cell == 0) {
        goto next_outer;
    }
    if (*visited != 0) {
        goto next_outer;
    }

    current = i;
    __asm__("" : "=r"(current) : "0"(current));
    cell = (UnkC590Cell *)(u32)(u8)current;
    if ((*(u8 *)func_8003B3D8((u32)cell, row) & 4) != 0) {
        goto next_outer;
    }
    if ((u32)cell == 0) {
        goto next_outer;
    }

chain_loop:
    cell = line[(u8)current];
    if (cell == 0) {
        goto next_outer;
    }
    if (tableBase[(u8)current] == 0) {
        tableBase[(u8)current] = i;
    }

    if (cell->unk28 == 0 && cell->unk2A > 0) {
        goto mark;
    }

    switch (cell->type) {
    case 0x5C:
        flag = (cell->unk48 == 0);
        break;
    case 0x57:
    case 0x58:
    case 0x59:
        flag = 1;
        break;
    case 0x5A:
    case 0x5B:
    case 0x5D:
    case 0x5E:
    default:
        flag = 0;
        break;
    }

    if (flag == 0) {
        goto after_mark;
    }
    if (cell->unk20 != 0) {
        goto after_mark;
    }

mark:
    gOrGridCellFlags((u8)current, row, 4);

after_mark:
    if (cell->next != 0) {
        if (line[cell->next] == 0) {
            goto next_outer;
        }
        if (tableBase[cell->next] == i) {
            tableBase[(u8)current] = 0;
            func_8003C500((u8)i, (u8)current, row);
            goto next_outer;
        }
    }

    current = cell->next;
    if (current != 0) {
        goto chain_loop;
    }

next_outer:
    visited++;
    i++;
    scan++;
    if (i < 0x100) {
        goto outer_loop;
    }
}
