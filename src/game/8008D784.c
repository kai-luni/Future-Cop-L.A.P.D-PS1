// Categories: BIOS > System Calls | Platform > CD-ROM
#include "game/types.h"

extern void func_8008DC40(void);
extern void func_8008DE20(void);

/**
 * @brief Runs the two CD driver init helpers in sequence.
 *
 * func_8008DC40 installs the driver's interrupt handlers, func_8008DE20 then
 * patches the CD subsystem base block (copies the three BIOS words into it and
 * flushes the cache). The order matters: the handlers must exist before the
 * block is written.
 * @par Globals
 * None directly.
 * @par Control flow
 * Two straight calls.
 * @par Calls
 * func_8008DC40 - handler setup. func_8008DE20 - base block patch.
 * @par Side effects
 * Installs handlers and rewrites the CD base block.
 * @par Callers
 * Unknown: no direct edge; part of the driver init chain.
 */
void func_8008D784(void)
{
    func_8008DC40();
    func_8008DE20();
}
