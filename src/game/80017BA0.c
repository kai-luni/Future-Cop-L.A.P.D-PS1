// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 gHeapPoolBase __asm__("D_8009FA9C");
extern s32 gFreeRangeTablePtr __asm__("D_8009F590");

extern void gHeapFreeRaw(s32 block) __asm__("func_8001E2C0");

/**
 * @brief Frees the two blocks of the heap layer created by func_80017BE8().
 *
 * D_8009FA9C (the main heap block) and D_8009F590 (the free-range table) are
 * released through func_8001E2C0(), each only when non-NULL, and both globals
 * are then cleared - so this is the safe, repeatable teardown (unlike most
 * other free wrappers in this build).
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076FE0 only, at the very end of the shutdown path.
 */
void gShutdownHeap(void) __asm__("func_80017BA0");

void gShutdownHeap(void)
{
    if (gHeapPoolBase != 0) {
        gHeapFreeRaw(gHeapPoolBase);
    }
    if (gFreeRangeTablePtr != 0) {
        gHeapFreeRaw(gFreeRangeTablePtr);
    }
    gHeapPoolBase = 0;
    gFreeRangeTablePtr = 0;
}
