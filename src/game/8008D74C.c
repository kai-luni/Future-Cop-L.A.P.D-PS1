// Categories: Platform > Memory Card | BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void func_8008DC30(void);
extern void func_8008D7B0(s32 value);
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");

/**
 * @brief Starts the BIOS card service and clears pad-clear mode inside a critical section.
 *
 * Brackets BIOS vector calls func_8008DC30 (B0:4B) and func_8008D7B0(0) (B0:5B) between EnterCriticalSection and ExitCriticalSection syscalls. Likely corresponds to PsyQ StartCARD() and ChangeClearPAD(0) during controller/card setup.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line sequence of four calls; no branches or loops.
 *
 * @par Calls
 * func_8008C9A0 - enter critical section (syscall 0, code 1); func_8008DC30 - BIOS B0:4B; func_8008D7B0 - BIOS B0:5B with arg 0; func_8008C9B0 - exit critical section (syscall 0, code 2).
 *
 * @par Side effects
 * Masks and restores interrupts via BIOS syscall 0 and updates BIOS B0:4B/B0:5B card/pad state.
 *
 * @par Callers
 * Confirmed: gInitMemoryCardBios (0x800200C8).
 */
void func_8008D74C(void)
{
    gEnterCriticalSection();
    func_8008DC30();
    func_8008D7B0(0);
    gExitCriticalSection();
}
