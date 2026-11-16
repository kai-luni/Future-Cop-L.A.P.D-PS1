// Categories: Platform > Status Polling
#include "../../include/game/types.h"

struct StatusState {
    u8 pad[4];
    volatile u16 status;
};

extern struct StatusState *D_8009C78C;

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
 */
void func_8008AE44(void)
{
    while ((D_8009C78C->status & 2) == 0) {
    }
}
