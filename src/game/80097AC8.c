// Categories: Platform > Sound | Platform > DMA
#include "game/types.h"

extern s32 gSpuAddrShift __asm__("D_8009EECC");
extern s32 gSpuEntryTableCapacity __asm__("D_8009EF04");
extern s32 gTableCursor __asm__("D_8009EF08");
extern u8 *gSpuEntryTable __asm__("D_8009EF0C");

/**
 * @brief Initialises the SPU RAM pool block table.
 *
 * @param count Number of pool entries; 0 aborts.
 * @param table Pool table (at least two u32 slots).
 * @return count on success, 0 when count <= 0.
 *
 * @par Confirmed
 * Writes table[0] = 0x40001010 (untouched-pool flag 0x40000000 plus first
 * free address 0x1010, the flag meaning documented in func_80097B28) and
 * table[1] = (0x10000 << gSpuAddrShift) - 0x1010 (pool size in units), then
 * publishes the table: gSpuEntryTable = table, gTableCursor = 0 (write
 * index), D_8009EF04 = count.
 *
 * @par Likely role
 * The pool setup of the sound streaming path func_80021B50: the allocator
 * func_80097B28, gSpuCompactPool and gSpuSplitPoolBlock maintain the same
 * table through gSpuEntryTable, gTableCursor and D_8009EF04.
 *
 * @par Unknown
 * What occupies SPU RAM 0x0000..0x100F; the pool starts above it.
 *
 * @par Callers
 * func_80021B50.
 */
s32 gSpuInitPool(s32 count, u8 *table) __asm__("func_80097AC8");

s32 gSpuInitPool(s32 count, u8 *table)
{
    s32 shift;

    if (count > 0) {
        shift = gSpuAddrShift;
        ((u32 *)table)[0] = 0x40001010;
        gSpuEntryTable = table;
        gTableCursor = 0;
        gSpuEntryTableCapacity = count;
        ((u32 *)table)[1] = (0x10000 << shift) - 0x1010;
        return count;
    }

    return 0;
}
