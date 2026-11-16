// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

typedef void (*CdDataCallback)(void);

extern CdDataCallback func_800914E0(s32 channel, CdDataCallback callback);

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
 */
CdDataCallback func_8008E5D8(CdDataCallback callback)
{
    return func_800914E0(3, callback);
}
