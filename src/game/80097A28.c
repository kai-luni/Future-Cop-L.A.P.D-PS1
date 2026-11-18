// Categories: Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern void gSpuInitSubsystem(s32 cold_start) __asm__("func_80096D58");

/**
 * @brief Warm-start display reinitialisation: calls gSpuInitSubsystem(0).
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Single call with cold_start = 0. gSpuInitSubsystem resets the GPU
 * (func_80091480), runs gSpuResetHardware(0), fills 0x18 words at D_8009EA8E
 * with 0xC000 (the warm-start-only branch), runs gSpuInitDmaEvent and then
 * rebuilds the display tables through gSpuWriteRegister.
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
    gSpuInitSubsystem(0);
}
