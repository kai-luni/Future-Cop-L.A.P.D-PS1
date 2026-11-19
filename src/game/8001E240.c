// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 gHeapHighWater __asm__("D_80010028");
extern s32 gHeapHighWaterPeak __asm__("D_8001002C");

extern s32 *gAllocHeapBlock(s32 size) __asm__("func_8008CA90");
s32 *gHeapAllocRaw(s32 size) __asm__("func_8001E240");


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
 *
 * @par Role and path
 * Memory-layer allocator not on the render path: requests size + 4 bytes from
 * func_8008CA90, advances gHeapHighWater (D_80010028) and updates
 * gHeapHighWaterPeak (D_8001002C), writes size into the 4-byte header, and
 * returns block + 1. Called by gAllocMemory (func_80017EBC), func_80017BE8,
 * and func_80076EB0.
 */
s32 *gHeapAllocRaw(s32 size)
{
    s32 *block;
    s32 next;

    if (size == 0) {
        return 0;
    }

    block = gAllocHeapBlock(size + 4);

    next = gHeapHighWater + 4;
    next += size;
    gHeapHighWater = next;
    if (next < gHeapHighWaterPeak) {
        next = gHeapHighWaterPeak;
    }
    gHeapHighWaterPeak = next;

    if (block == 0) {
        return 0;
    }

    *block = size;
    return block + 1;
}
