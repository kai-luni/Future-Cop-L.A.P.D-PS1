// Categories: Resources > Memory Management
#include "game/types.h"

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases a pool block (thin wrapper around func_800181D4).
 *
 * @param block The pool allocation to free.
 * @return Nothing.
 *
 * @par Confirmed
 * Tail call to func_800181D4(block), the pool release helper; no other code.
 *
 * @par Likely role
 * A stable free entry point for the effect/beam handlers so their cleanup
 * case can call one address regardless of the underlying allocator.
 *
 * @par Callers
 * func_80071A68, func_8007377C, func_80073BBC.
 */
void func_80070710(void *block)
{
    gFreeBlock(block);
}
