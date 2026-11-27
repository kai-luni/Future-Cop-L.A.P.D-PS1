// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern void gResetCdHardware(void) __asm__("func_8008F6D8");
void gResetCdController(void) __asm__("func_8008E0B8");


/**
 * @brief Clears pending CD-ROM interrupts (driver reset).
 *
 * One-line forwarder to func_8008F6D8, which resets the CD status register and
 * the bus delay after an aborted or failed command.
 * @par Globals
 * None directly.
 * @par Control flow
 * None.
 * @par Calls
 * func_8008F6D8 - interrupt/status reset.
 * @par Side effects
 * Clears CD hardware interrupt flags.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 *
 * @par Role and path
 * Calls func_8008F6D8 to acknowledge pending CD-ROM hardware interrupts,
 * reset D_8009CB84[0..2], and restore COM_DELAY. Accesses no globals directly
 * and has no direct callers in src/.
 */
void gResetCdController(void)
{
    gResetCdHardware();
}
