// Categories: BIOS > System Calls | Platform > CD-ROM
#include "game/types.h"

extern void func_8008DC40(void);
extern void gPatchCdKernelBlock(void) __asm__("func_8008DE20");
void gInitCdDriverChain(void) __asm__("func_8008D784");


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
 *
 * @par Role and path
 * Runs the two CD driver initialization helpers func_8008DC40 and
 * func_8008DE20 in sequence. Accesses no globals directly; func_8008DC40
 * dispatches BIOS B0:4C and func_8008DE20 patches the kernel device block and
 * flushes cache. Driven by func_800200F0.
 */
void gInitCdDriverChain(void)
{
    func_8008DC40();
    gPatchCdKernelBlock();
}
