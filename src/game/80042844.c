// Categories: Resources > Memory Management | Graphics > Effects
#include "game/types.h"

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases a memory block through func_800181D4.
 *
 * Thin wrapper passing block directly to the general deallocator func_800181D4; sits in the effects module between func_800427E8 and func_80042864.
 *
 * @param block Pointer previously allocated via func_80017EBC.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single call to func_800181D4(block) and return.
 *
 * @par Calls
 * func_800181D4 - returns block to its fixed pool, free-range table, or general heap.
 *
 * @par Side effects
 * Frees block via func_800181D4.
 *
 * @par Callers
 * Confirmed: none in asm/us/main.s or analysis/edges.json / Likely: unused helper or stripped effect-object destructor / Unknown: any runtime indirect caller.
 */
void func_80042844(void *block)
{
    gFreeBlock(block);
}
