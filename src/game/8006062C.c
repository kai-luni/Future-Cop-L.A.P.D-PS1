// Categories: Utilities > Mathematics | Gameplay > AI
#include "game/types.h"

extern void func_8001846C(u32 id, s32 *first, s32 *second);

/**
 * @brief Returns the cosine value for a 4096-step angle, clamped to -0x10000 at or above half a turn.
 *
 * Rejects unsigned angle indices >= 0x800 (2048 steps, i.e. >= 180 degrees or wrapped negative values) by returning -0x10000 directly; otherwise delegates to func_8001846C and discards the sine output. Category Utilities > Mathematics | Gameplay > AI follows the trig helper func_8001846C and caller func_80060668.
 *
 * @param id Unsigned angle index in 1/4096-turn units (clamped when >= 0x800).
 * @return Scaled cosine from func_8001846C, or -0x10000 when id >= 0x800.
 *
 * @par Globals
 * None directly (func_8001846C reads quarter-wave table D_8009A234).
 *
 * @par Control flow
 * Single unsigned threshold check (`id >= 0x800`) returning -0x10000 early, followed by one call and return of `second`.
 *
 * @par Calls
 * func_8001846C - computes sine into local `first` and cosine into `second` from D_8009A234.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80060668 (0x80060700 and 0x80060724 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_8006062C(u32 id)
{
    s32 first;
    s32 second;

    if (id >= 0x800) {
        return -0x10000;
    }
    func_8001846C(id, &first, &second);
    return second;
}
