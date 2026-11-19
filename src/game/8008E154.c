// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern s32 gCdSyncLoop(s32 mode, u8 *result) __asm__("func_8008ECFC");
s32 gCdSync(s32 mode, u8 *result) __asm__("func_8008E154");


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
 *
 * @par Role and path
 * Forwards mode and result directly to func_8008ECFC to poll once (mode != 0)
 * or block until the active CD-ROM command completes (mode == 0). Accesses no
 * globals directly and is called by gCdStreamService.
 */
s32 gCdSync(s32 mode, u8 *result)
{
    return gCdSyncLoop(mode, result);
}
