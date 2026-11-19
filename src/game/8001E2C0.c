// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 gHeapHighWater __asm__("D_80010028");
extern void gFreeHeapBlock(s32 *header) __asm__("func_8008CB40");
void gHeapFreeRaw(s32 block) __asm__("func_8001E2C0");


/**
 * @brief General-purpose free: returns a block to the low-level allocator.
 *
 * @param block Pointer returned by func_8001E240(). NULL is ignored.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * The size is read from the header word at block[-1], the high-water mark
 * D_80010028 is rewound by size + 4 and the header is handed to func_8008CB40(),
 * which merges it back into the block list.
 *
 * @par Globals
 * D_80010028 (current heap end).
 *
 * @par Callers
 * func_800181D4 (for blocks that are in neither a pool nor a free range),
 * func_80076FE0, func_80017BA0 and the unit at 0x80017BA4.
 *
 * @par Note
 * The high-water mark D_8001002C is not rewound, which is consistent with
 * "highest end ever reached" and is what the pair is used for.
 *
 * @par Role and path
 * Memory-layer deallocator not on the render path: ignores NULL, reads size
 * from ((s32 *)block)[-1], subtracts size + 4 from gHeapHighWater
 * (D_80010028), and passes block - 1 to func_8008CB40. Called by gFreeBlock
 * (func_800181D4), func_80076FE0, and func_80017BA0.
 */
void gHeapFreeRaw(s32 block)
{
    if (block != 0) {
        s32 size = ((s32 *)block)[-1];

        gHeapHighWater = gHeapHighWater - size - 4;
        gFreeHeapBlock((s32 *)block - 1);
    }
}
