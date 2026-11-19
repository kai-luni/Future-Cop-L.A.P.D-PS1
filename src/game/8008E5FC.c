// Categories: Platform > CD-ROM | Platform > Device Wait
#include "game/types.h"

extern s32 gWaitCdDmaComplete(s32 mode) __asm__("func_8008FAC8");
s32 gCdDataSync(s32 mode) __asm__("func_8008E5FC");


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
 *
 * @par Role and path
 * Forwards mode to func_8008FAC8 to poll once (mode != 0) or block (mode ==
 * 0) on the CD-ROM data-ready state. Accesses no globals directly and is
 * called by func_80090E00.
 */
s32 gCdDataSync(s32 mode)
{
    return gWaitCdDmaComplete(mode);
}
