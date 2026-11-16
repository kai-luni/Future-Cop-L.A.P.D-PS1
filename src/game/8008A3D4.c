// Categories: Input > State Checks
#include "game/intc_globals.h"

typedef struct {
    unsigned int first;
    unsigned int second;
} Flags;

extern Flags *gInterruptStatusPtr __asm__("D_8009C788");
extern void (*D_8009C750)(void);

/**
 *  @brief Tests two flag bits and invokes an optional callback when both are set.
 *
 *  Checks bit zero in each word of the shared flags record before dispatch.
 *  A missing callback does not prevent a successful result.
 *  @return One if both bits are set, otherwise zero.
 *  @par Globals
 *  D_8009C788 points to the flags; D_8009C750 holds the optional callback.
 *  @par Control flow
 *  Returns early on either clear bit, then conditionally calls the callback.
 *  @par Calls
 *  The D_8009C750 callback is invoked when present and both bits are set.
 *  @par Side effects
 *  The callback may have effects; no state is directly written here.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8008A3D4(void)
{
    if (!(gInterruptStatusPtr->second & 1))
        return 0;
    if (!(gInterruptStatusPtr->first & 1))
        return 0;
    if (D_8009C750 != 0)
        D_8009C750();
    return 1;
}
