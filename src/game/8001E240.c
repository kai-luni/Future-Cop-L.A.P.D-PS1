// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 D_80010028;
extern s32 D_8001002C;

extern s32 *func_8008CA90(s32 size);

/**
 * @brief General-purpose allocator: bumps a block out of the raw heap and
 *        records a size header in front of it.
 *
 * @param size Number of usable bytes; 0 returns NULL without touching anything.
 *
 * @return Pointer to the usable area (block + 4) or NULL when the underlying
 *         allocator func_8008CA90(size + 4) failed.
 *
 * @par Confirmed flow
 * 1. block = func_8008CA90(size + 4) - the low-level "take the next free area"
 *    call, which already reserves the 4-byte header.
 * 2. The high-water marks are advanced: D_80010028 += 4 + size and
 *    D_8001002C = max(D_8001002C, D_80010028), so the pair tracks the current
 *    end and the highest end ever reached.
 * 3. The header word receives @c size and the caller gets block + 1.
 *
 * @par Globals
 * D_80010028 (current heap end), D_8001002C (heap high-water mark).
 *
 * @par Callers
 * func_80017EBC (mode 0), func_80017BE8, func_80076EB0 and the whole heap
 * layer. Call sites pass a second argument (0 or 3) which this unit does not
 * read - the rebuild is byte-exact without it.
 */
s32 *func_8001E240(s32 size)
{
    s32 *block;
    s32 next;

    if (size == 0) {
        return 0;
    }

    block = func_8008CA90(size + 4);

    next = D_80010028 + 4;
    next += size;
    D_80010028 = next;
    if (next < D_8001002C) {
        next = D_8001002C;
    }
    D_8001002C = next;

    if (block == 0) {
        return 0;
    }

    *block = size;
    return block + 1;
}
