// Categories: Resources > Memory Pools
#include "game/types.h"

extern void *gFixedPool3 __asm__("D_8009FA4C");
extern void *gFixedPool2 __asm__("D_8009FAAC");
extern void *gFixedPool1 __asm__("D_8009F9D0");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases the three fixed-size pools created by func_80017C68().
 *
 * D_8009FA4C, D_8009FAAC and D_8009F9D0 are handed to the shared deallocator
 * in that order. The order is the reverse of the creation order inside
 * func_80017C68 and the pointers are not cleared, so the pair must not run
 * twice without a new set-up.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80077088 (per-frame), func_800249BC and func_80024D14 (module
 * teardown) - so every screen start/end rebuilds the pool layer.
 */
void func_80017D20(void)
{
    gFreeBlock(gFixedPool3);
    gFreeBlock(gFixedPool2);
    gFreeBlock(gFixedPool1);
}
