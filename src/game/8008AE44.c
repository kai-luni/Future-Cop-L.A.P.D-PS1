// Categories: Platform > Status Polling
#include "game/sio_globals.h"

#include "../../include/game/types.h"

struct StatusState {
    u8 pad[4];
    volatile u16 status;
};

extern struct StatusState *gSio0RegBasePtr __asm__("D_8009C78C");
void gWaitSio0TxReady(void) __asm__("func_8008AE44");


/**
 *  @brief Waits until bit 1 of a status field is set.
 *
 *  Polls a volatile status field through the global state pointer.
 *  The wait has no timeout or other exit condition.
 *  @par Globals
 *  D_8009C78C points to the structure containing the status field.
 *  @par Control flow
 *  Repeatedly reads status until its 0x2 bit is nonzero.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Performs repeated volatile reads and may wait indefinitely.
 *  @par Callers
 *  Confirmed: func_8008A6CC, func_8008BA34.
 *
 * @par Role and path
 * Spins on gSio0RegBasePtr->status & 2 until bit 1 (SIO0 TX ready) becomes
 * nonzero, with no timeout. Called by func_8008A6CC and func_8008BA34.
 */
void gWaitSio0TxReady(void)
{
    while ((gSio0RegBasePtr->status & 2) == 0) {
    }
}
