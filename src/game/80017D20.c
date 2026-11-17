// Categories: Resources > Memory Pools
#include "game/types.h"

extern void *gPool3Size __asm__("D_8009FA4C");
extern void *gPool2Size __asm__("D_8009FAAC");
extern void *gPool1Size __asm__("D_8009F9D0");

extern void func_800181D4(void *block);

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
    func_800181D4(gPool3Size);
    func_800181D4(gPool2Size);
    func_800181D4(gPool1Size);
}
