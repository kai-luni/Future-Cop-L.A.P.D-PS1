// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

typedef void (*CdDataCallback)(void);

extern CdDataCallback gSetDmaCallback(s32 channel, CdDataCallback callback) __asm__("func_800914E0");
CdDataCallback gSetCdDmaCallback(CdDataCallback callback) __asm__("func_8008E5D8");


/**
 * @brief Installs a callback on CD-ROM interrupt channel 3.
 *
 * One-line wrapper: the callback goes to the interrupt manager func_800914E0
 * with channel 3, the CD data-ready/command interrupt of the asynchronous read
 * path. The previous handler is returned unchanged.
 *
 * @param callback Function to run when the channel fires.
 * @return The handler installed before.
 *
 * @par Globals
 * None directly; the manager keeps the table.
 *
 * @par Control flow
 * None.
 *
 * @par Calls
 * func_800914E0 - callback registration.
 *
 * @par Side effects
 * Replaces the channel-3 handler.
 *
 * @par Callers
 * Confirmed: func_80090E00 (read teardown).
 *
 * @par Role and path
 * Registers callback on DMA channel 3 by calling func_800914E0(3, callback)
 * and returns the previous callback pointer. Driven by func_80090B48 and
 * func_80090E00.
 */
CdDataCallback gSetCdDmaCallback(CdDataCallback callback)
{
    return gSetDmaCallback(3, callback);
}
