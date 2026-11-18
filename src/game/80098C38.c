// Categories: Platform > Audio | Platform > Interrupts
#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

extern void func_800914B0(s32 interrupt, SpuIrqCallback callback);

/**
 * @brief Installs an SPU interrupt callback.
 *
 * Registers the callback with func_800914B0 on interrupt number 9, which is the
 * SPU RAM-transfer interrupt. Wrapping the number keeps callers independent of
 * the interrupt numbering.
 *
 * @param callback Handler run on each SPU transfer interrupt.
 * @par Globals
 * None directly; the interrupt manager keeps the table.
 * @par Control flow
 * None.
 * @par Calls
 * func_800914B0 - interrupt handler registration.
 * @par Side effects
 * Replaces the SPU interrupt handler.
 * @par Callers
 * Confirmed: gSpuInstallIrqCallback (0x80098C18), which wraps this call.
 */
void gSpuSetIrqCallback(SpuIrqCallback callback) __asm__("func_80098C38");

void gSpuSetIrqCallback(SpuIrqCallback callback)
{
    func_800914B0(9, callback);
}
