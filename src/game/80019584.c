// Categories: Resources > Memory Management
#include "game/types.h"

extern void func_800181D4(void);

/**
 * @brief Thin wrapper around the upper allocator's free entry
 *        func_800181D4.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Calls func_800181D4 with no arguments of its own (the block pointer
 * passes through the register convention).
 *
 * @par Likely role
 * A teardown shim used by the reset paths (func_80029E64, func_80029EB8,
 * func_8003FAD4): keeps the call site stable while the free implementation
 * sits in a different linkage group.
 *
 * @par Callers
 * func_80029E64, func_80029EB8, func_8003FAD4.
 */
void func_80019584(void)
{
    func_800181D4();
}
