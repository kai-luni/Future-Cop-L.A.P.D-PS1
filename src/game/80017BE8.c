// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 D_8009F59C;
extern s32 D_8009F5A0;
extern s32 gHeapMainBlockSize __asm__("D_8009F9E8");
extern s32 gHeapPoolBase __asm__("D_8009FA9C");

extern s32 func_8001E240(s32, s32);

/**
 * @brief Initialises the heap: the free-range table and the big main block.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. D_8009F59C = 0x203 ("no range"), D_8009F5A0 = 0x182 ("invalid"),
 *    D_8009F590 and D_8009FA9C cleared.
 * 2. D_8009F590 = func_8001E240(0x810, 3) - 0x810 bytes for the free-range
 *    table (512 words of start/end pairs plus header).
 * 3. D_8009F9E8 = 0x15A000 and the main block is requested with
 *    func_8001E240(0x15A000, 3). If the request fails, the size is reduced by
 *    0x1000 and retried until it succeeds - so the heap shrinks to whatever is
 *    actually free, and D_8009F9E8 always holds the size that was granted.
 *
 * @par Globals
 * D_8009F590 (range table), D_8009F59C/D_8009F5A0 (sentinel values 0x203/0x182),
 * D_8009F9E8 (granted heap size), D_8009FA9C (heap block).
 *
 * @par Callers
 * func_80076EB0, once during boot; func_80017BA0 undoes it.
 *
 * @par Note
 * The third argument that other call sites pass to func_8001E240 (0 or 3) is
 * a small-data/allocation-mode request that the callee does not read; the
 * rebuild is byte-exact without it.
 */
void gInitHeap(void) __asm__("func_80017BE8");

void gInitHeap(void)
{
    D_8009F59C = 0x203;
    gHeapPoolBase = 0;
    gFreeRangeTablePtr = 0;
    D_8009F5A0 = 0x182;

    gFreeRangeTablePtr = func_8001E240(0x810, 3);
    gHeapMainBlockSize = 0x15A000;
    gHeapPoolBase = func_8001E240(0x15A000, 3);

    while (gHeapPoolBase == 0) {
        gHeapMainBlockSize -= 0x1000;
        gHeapPoolBase = func_8001E240(gHeapMainBlockSize, 3);
    }
}
