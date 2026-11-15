// Categories: Platform > Sound | Platform > DMA
#include "game/types.h"

extern s32 D_8009EECC;
extern s32 D_8009EF04;
extern s32 D_8009EF08;
extern u8 *D_8009EF0C;

/**
 * @brief Arms an SPU DMA transfer table with a header and a size word.
 *
 * @param count Number of transfer entries; 0 aborts.
 * @param table Destination table (at least two u32 slots).
 * @return count on success, 0 when count <= 0.
 *
 * @par Confirmed
 * Writes table[0] = 0x40001010 and table[1] = (0x10000 << D_8009EECC) -
 * 0x1010, then publishes the transfer state: D_8009EF0C = table,
 * D_8009EF08 = 0 (index), D_8009EF04 = count.
 *
 * @par Likely role
 * The queue setup of the sound streaming path func_80021B50: the header is
 * the SPU DMA control word, the second word encodes the block size from the
 * shift latch D_8009EECC.
 *
 * @par Unknown
 * The exact bit layout of 0x40001010 and who consumes D_8009EF04..EF0C.
 *
 * @par Callers
 * func_80021B50.
 */
s32 func_80097AC8(s32 count, u8 *table)
{
    s32 shift;

    if (count > 0) {
        shift = D_8009EECC;
        ((u32 *)table)[0] = 0x40001010;
        D_8009EF0C = table;
        D_8009EF08 = 0;
        D_8009EF04 = count;
        ((u32 *)table)[1] = (0x10000 << shift) - 0x1010;
        return count;
    }

    return 0;
}
