// Categories: Resources > Memory
#include "game/types.h"

extern void *D_8009F518;
extern void func_800181D4(void *block);

/**
 *  @brief Releases the stored block if one is present.
 *
 *  This cleanup routine passes the global block pointer to the release
 *  function and clears it so the block is not released again through it.
 *  @par Globals
 *  D_8009F518 holds the block pointer and is cleared after release.
 *  @par Control flow
 *  A null pointer skips both the call and the assignment.
 *  @par Calls
 *  func_800181D4 releases the non-null block.
 *  @par Side effects
 *  Releases the block and clears its global pointer when present.
 *  @par Callers
 *  Confirmed: func_800898C8.
 */
void func_800898E8(void)
{
    if (D_8009F518 != (void *)0) {
        func_800181D4(D_8009F518);
        D_8009F518 = (void *)0;
    }
}
