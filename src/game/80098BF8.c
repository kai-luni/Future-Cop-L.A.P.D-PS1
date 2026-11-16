// Categories: Platform > Sound > Interrupts | Platform > Interrupts
#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

extern SpuIrqCallback D_8009EEE0;
extern void func_80098C38(SpuIrqCallback callback);

/**
 * @brief Installs the SPU interrupt handler, skipping redundant installs.
 *
 * D_8009EEE0 caches the handler currently installed; when the request matches it the body returns without touching the driver, which keeps the callers from re-entering the interrupt setup every frame.
 *
 * @param callback New SPU interrupt handler; 0 removes the hook.
 * @return The handler that was installed before the call.
 *
 * @par Globals
 * D_8009EEE0 - cached interrupt handler; read, compared against the argument, written before the driver call.
 *
 * @par Control flow
 * One compare; the driver call is taken only when the handler differs.
 *
 * @par Calls
 * func_80098C38 - forwards the handler as interrupt 9 to func_800914B0, the driver's interrupt-hook dispatcher.
 *
 * @par Side effects
 * The SPU interrupt vector changes; a redundant call leaves both the cache and the vector untouched. func_80097A48 clears the cache to 0 through this path at shutdown.
 *
 * @par Callers
 * Confirmed: func_80087750 (0x8008777C) and func_80087824 (0x80087908), sound shutdown and init, func_800228F4 (0x80022940), func_80022A64 (0x80022A9C).
 * Unknown: two calls at 0x80086AE8 and 0x80086C50 from a routine that is not in the matching set.
 */
SpuIrqCallback func_80098BF8(SpuIrqCallback callback)
{
    SpuIrqCallback previous;

    previous = D_8009EEE0;
    if (callback != previous) {
        D_8009EEE0 = callback;
        func_80098C38(callback);
    }
    return previous;
}
