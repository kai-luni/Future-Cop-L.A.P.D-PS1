void gEnterCriticalSection(void) __asm__("func_8008C9A0");

// Categories: BIOS > System Calls | Platform > Interrupts
/**
 * @brief Enters a PS1 BIOS critical section by invoking syscall 0 with function code 1 (EnterCriticalSection).
 *
 * Confirmed: sets $a0 = 1 and executes `syscall 0` before returning (`jr $ra; nop` at 0x8008C9A0 in asm/us/main.s). Callers func_80095DB0 and gSpuInitDmaEvent bracket BIOS table/event/interrupt setup between func_8008C9A0 and func_8008C9B0. Category BIOS > System Calls | Platform > Interrupts follows `syscall 0` kernel dispatch and interrupt masking.
 * Likely: disables hardware interrupts via the PS1 kernel so shared interrupt/event/DMA state can be updated atomically; all C callers ignore any kernel return in $v0.
 * Unknown: none.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; alters PS1 kernel/COP0 interrupt mask state via BIOS syscall 1.
 *
 * @par Control flow
 * Straight-line inline assembly: load $a0 = 1, `syscall 0`, return.
 *
 * @par Calls
 * None (PS1 kernel `syscall 0`).
 *
 * @par Side effects
 * Masks interrupts in PS1 kernel critical-section state.
 *
 * @par Callers
 * Confirmed: func_8008A5BC, func_8008A688, func_8008D6E0, func_8008D74C, func_8008D7DC, func_8008D86C, func_8008D964, func_8008D9DC, func_8008DB28, func_8008DBB0, func_8008DCE8, func_8008DD7C, func_8008DE20, func_80091A04, func_80095DB0, gSpuInitDmaEvent, gSpuShutdownDmaEvent / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Invokes PS1 kernel syscall 0 with $a0 = 1 (EnterCriticalSection) to mask
 * interrupts during atomic BIOS, event, and DMA updates. Paired with
 * func_8008C9B0 by callers including func_8008A5BC, func_80095DB0, and
 * gSpuInitDmaEvent.
 */
void gEnterCriticalSection(void)
{
    __asm__ volatile("addiu $a0, $zero, 0x1\n\tsyscall 0");
}
