// Categories: Resources > Memory Pools
#include "game/types.h"

extern void func_80017D20(void);
void gFreeBlitPools(void) __asm__("func_80077088");


/**
 * @brief Releases the three fixed-size pools installed by func_80077050().
 *
 * func_80017D20() frees D_8009FA4C, D_8009FAAC and D_8009F9D0 through the
 * shared deallocator. The pointers are not cleared, so the pair
 * func_80077050/80077088 must not be run twice without a new set-up in
 * between - which is exactly how the main loop uses them (set-up before the
 * blits, release after).
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076A2C only, at the end of the boot sequence and after each frame
 * that drew something.
 */
void gFreeBlitPools(void)
{
    func_80017D20();
}
