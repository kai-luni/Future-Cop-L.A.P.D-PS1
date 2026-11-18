// Categories: BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

/**
 * @brief Exits a PS1 BIOS critical section by invoking syscall 0 with function code 2 (ExitCriticalSection).
 *
 * Confirmed: sets $a0 = 2 and executes `syscall 0` before returning (`jr $ra; nop` at 0x8008C9B0 in asm/us/main.s). Callers pair it with func_8008C9A0 to restore interrupts after BIOS table, event, or driver updates. Category BIOS > System Calls | Platform > Interrupts follows `syscall 0` kernel dispatch and interrupt restoration.
 * Likely: re-enables hardware interrupts via the PS1 kernel upon leaving the critical section.
 * Unknown: none.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; alters PS1 kernel/COP0 interrupt mask state via BIOS syscall 2.
 *
 * @par Control flow
 * Straight-line inline assembly: load $a0 = 2, `syscall 0`, return.
 *
 * @par Calls
 * None (PS1 kernel `syscall 0`).
 *
 * @par Side effects
 * Unmasks interrupts in PS1 kernel critical-section state.
 *
 * @par Callers
 * Confirmed: func_8008A5BC, func_8008A688, func_8008D6E0, func_8008D74C, func_8008D7DC, func_8008D86C, func_8008D964, func_8008D9DC, func_8008DBB0, func_8008DE20, func_80091614, func_80091AA4, func_80095DB0, gSpuInitDmaEvent, gSpuShutdownDmaEvent / Likely: none / Unknown: none.
 */
void func_8008C9B0(void)
{
    __asm__ volatile("addiu $a0, $zero, 0x2\n\tsyscall 0");
}
