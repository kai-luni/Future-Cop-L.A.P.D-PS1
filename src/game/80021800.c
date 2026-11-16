// Categories: Resources > Offset Tables
#include "../../include/game/types.h"

/**
 *  @brief Resolves an indexed relative offset within a resource block.
 *
 *  Reads a 32-bit offset from the table beginning at base plus 0x10.
 *  A zero table entry represents an absent address.
 *  @param base Base address of the block containing the offset table.
 *  @param index Index of the 32-bit table entry.
 *  @return Base plus the stored offset, or null if the offset is zero.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Branches on whether the selected offset is zero; no bounds check is performed.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80021F98.
 */
void *func_80021800(void *base, u32 index)
{
    u32 offset = *(u32 *)((u8 *)base + 0x10 + index * 4);

    if (offset == 0) {
        return 0;
    }
    return (u8 *)base + offset;
}
