// Categories: Platform > Memory Card | BIOS > Events | Platform > Interrupts
#include "game/types.h"

extern void gReleaseMcEvents(void) __asm__("func_8002036C");
extern void gReleaseMcCallback(void) __asm__("func_8002059C");

void gReleaseCardEventServices(void) __asm__("func_8002078C");

/**
 * @brief Releases the memory-card event and callback services.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gReleaseMcEvents followed by gReleaseMcCallback, then returns.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gReleaseProfileCardOperation.
 *
 * @par Role and path
 * The memory-card shutdown path calls this wrapper when leaving the card
 * service.
 *
 * @par Unknown
 * The saved callback and event handles are maintained by the callees.
 */
void gReleaseCardEventServices(void)
{
    gReleaseMcEvents();
    gReleaseMcCallback();
}
