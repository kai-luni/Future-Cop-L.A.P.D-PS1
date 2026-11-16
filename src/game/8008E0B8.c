// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern void func_8008F6D8(void);

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
 */
void func_8008E0B8(void)
{
    func_8008F6D8();
}
