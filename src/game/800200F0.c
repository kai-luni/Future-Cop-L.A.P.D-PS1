// Categories: Platform > BIOS | Platform > Interrupts
#include "game/types.h"

extern void func_8008C9C0(void);
extern void func_8008D784(void);

/**
 * @brief Re-runs the BIOS-level device teardown around the interface reset.
 *
 * func_8008C9C0 clears the COP0 interrupt-enable bit, func_8008D784 issues BIOS entry B0 function 0x4C (func_8008DC40) and then B0 function 0x56, whose result points at the kernel device block that func_8008DE20 patches with three words at +0x70; the closing func_8008C9C0 keeps interrupts off.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_800A3E40 - kernel return address saved by func_8008DE20 before the B0 call. D_8008DE90 through D_8008DE9C - the three words copied into the kernel block.
 *
 * @par Control flow
 * Linear, no branches. The two func_8008C9C0 calls are not a matched pair: the second only re-disables what the BIOS calls may have re-enabled.
 *
 * @par Calls
 * func_8008C9C0 - disables interrupts through COP0 status. func_8008D784 - runs the two BIOS calls of the reset sequence.
 *
 * @par Side effects
 * Interrupts end up disabled and kernel device-table words are overwritten, which affects every later BIOS-level device access in the frame.
 *
 * @par Callers
 * Confirmed: func_8001E540 (0x8001E548), first step of the controller/device shutdown.
 */
void func_800200F0(void)
{
    func_8008C9C0();
    func_8008D784();
    func_8008C9C0();
}
