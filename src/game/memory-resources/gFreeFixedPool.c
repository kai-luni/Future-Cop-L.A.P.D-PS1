// Categories: Resources > Memory Management
#include "game/types.h"

extern void gFreeBlock(void) __asm__("func_800181D4");

void gFreeFixedPool(void) __asm__("func_80019584");

/**
 * @brief Passes a pool allocation to the shared heap free routine.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The JAL reaches gFreeBlock with the incoming $a0 unchanged. The C
 * reconstruction preserves this pass-through with a void signature.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80029E64, func_80029EB8,
 * gFinishLevelLoad, gReleaseMapLightData, func_80083428.
 *
 * @par Role and path
 * Pool-owning subsystems use this wrapper to release a block created by
 * gCreateFixedPool.
 *
 * @par Unknown
 * The wrapper contains no pool-specific destructor or cleanup policy.
 */
void gFreeFixedPool(void)
{
    gFreeBlock();
}
