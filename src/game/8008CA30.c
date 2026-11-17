// Categories: Platform > Memory | Resources > Memory Management
#include "game/types.h"

extern u32 *gSessionBlockBase __asm__("D_8009C7D0");

/**
 * @brief Seeds the low-level block allocator with one free block covering the
 *        given area.
 *
 * @param arg0 Start of the free area. NULL is ignored.
 * @param arg1 Size in bytes. 0 is ignored; otherwise the size is rounded *down*
 *             to a multiple of 8.
 *
 * @return Nothing.
 *
 * @par Confirmed layout
 * With the size rounded down, the function writes:
 * - at base + size - 8: the block start (self reference as "previous"),
 * - at base + size - 4: 0 (end of list),
 * - at base + 0: base + size - 8 (link to the terminator),
 * - at base + 4: ((size + 7) >> 3) - 1, the block length in 8-byte units.
 * D_8009C7D0 is set to base, i.e. "current block".
 *
 * @par Likely role
 * This is the initialisation of the simple linked-block allocator whose
 * allocation/free halves are func_8008CA90 and func_8008CB40. The entrypoint
 * computes the free area (D_8009EA04/D_8009EA08) and func_80076EB0 hands it to
 * this function before anything else allocates.
 *
 * @par Callers
 * func_80076EB0 only.
 */
void func_8008CA30(u32 *arg0, u32 arg1)
{
    u32 *ptr;
    register u32 count __asm__("$2");

    if ((arg0 == 0) || (arg1 == 0)) {
        return;
    }

    while (arg1 & 7) {
        arg1--;
    }

    ptr = (u32 *)((u8 *)arg0 + arg1);
    ptr[-2] = (u32)arg0;
    ptr -= 2;
    ptr[1] = 0;
    arg0[0] = (u32)ptr;

    count = ((arg1 + 7) >> 3) - 1;
    gSessionBlockBase = arg0;
    arg0[1] = count;
}
