// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern s32 func_8008ECFC(s32 mode, u8 *result);

/**
 * @brief Waits for the CD command to settle (CD_sync wrapper).
 *
 * Forwards both arguments to func_8008ECFC, the blocking command synchroniser,
 * so the mode selects "wait for completion" and the result buffer receives the
 * status bytes.
 *
 * @param mode 0 blocks until the command finished, non-zero polls once.
 * @param result Buffer for the returned status.
 * @return The command status.
 * @par Globals
 * None directly.
 * @par Control flow
 * None.
 * @par Calls
 * func_8008ECFC - CD_sync.
 * @par Side effects
 * Completes or aborts the running CD command.
 * @par Callers
 * Confirmed: gCdStreamService (device start/stop).
 */
s32 func_8008E154(s32 mode, u8 *result)
{
    return func_8008ECFC(mode, result);
}
