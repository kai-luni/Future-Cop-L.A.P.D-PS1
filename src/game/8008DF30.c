// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern u32 func_8008F8E8(void);
extern u32 func_8008F7AC(void);

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
 */
u32 func_8008DF30(void)
{
    if (func_8008F8E8() != 0)
        return 0;
    return func_8008F7AC() == 0;
}
