// Categories: Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern void func_80096D58(s32 cold_start);

/**
 * @brief Warm-start display reinitialisation: calls func_80096D58(0).
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Single call with cold_start = 0. func_80096D58 resets the GPU
 * (func_80091480), runs func_80096EB8(0), fills 0x18 words at D_8009EA8E
 * with 0xC000 (the warm-start-only branch), runs func_80096E40 and then
 * rebuilds the display tables through func_800977C4.
 *
 * @par Likely role
 * The display re-init used when the sound/CD subsystem func_80021B50
 * resumes after a pause: the tables are refreshed without the cold-start
 * probe sequence.
 *
 * @par Callers
 * func_80021B50.
 */
void func_80097A28(void)
{
    func_80096D58(0);
}
