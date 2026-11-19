// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern u32 gInitCdSubsystem(void) __asm__("func_8008F8E8");
extern u32 gInitCdAudioMix(void) __asm__("func_8008F7AC");
u32 gInitCdHardware(void) __asm__("func_8008DF30");


/**
 * @brief Checks whether the CD subsystem is ready for a command.
 *
 * Returns 0 when func_8008F8E8 already reports an initialised/active driver,
 * otherwise the result of func_8008F7AC == 0 (the "no error" test). The pairing
 * lets the caller distinguish "needs init" from "init failed".
 * @return 0 when initialised or failed, 1 when init is still required.
 * @par Globals
 * None directly.
 * @par Control flow
 * Two calls with an early exit.
 * @par Calls
 * func_8008F8E8 - driver state. func_8008F7AC - last driver error.
 * @par Side effects
 * None observed.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 *
 * @par Role and path
 * Calls func_8008F8E8 to initialize CD-ROM hardware and returns 0 if it
 * reports a non-zero error. Otherwise calls func_8008F7AC to configure CD/SPU
 * audio registers and returns 1 when it returns 0. Driven by func_8008DEA0.
 */
u32 gInitCdHardware(void)
{
    if (gInitCdSubsystem() != 0)
        return 0;
    return gInitCdAudioMix() == 0;
}
