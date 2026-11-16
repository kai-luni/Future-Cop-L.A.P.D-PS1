// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern s32 func_8008FAC8(s32 mode);

/**
 * @brief Polls the CD-ROM reply state through the mode wrapper.
 *
 * Thin forwarder to func_8008FAC8: the argument selects a single pass (non-zero)
 * or a retrying wait (0). The wrapper exists so the driver can swap the wait
 * implementation without touching callers.
 *
 * @param mode 0 keeps retrying, non-zero performs one pass.
 * @return -1 on timeout, 0 while the reply bit is clear, 1 when it is set.
 *
 * @par Globals
 * None directly.
 *
 * @par Control flow
 * None.
 *
 * @par Calls
 * func_8008FAC8 - the actual wait loop.
 *
 * @par Side effects
 * Same as func_8008FAC8.
 *
 * @par Callers
 * Confirmed: func_80090E00 (read abort path).
 */
s32 func_8008E5FC(s32 mode)
{
    return func_8008FAC8(mode);
}
