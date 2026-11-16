// Categories: BIOS > System Calls | Platform > Interrupts
extern void func_8008C9A0(void);
extern void func_8008CA10(int, void *);
extern void func_8008C9B0(void);
extern char D_800A3E00[];

/**
 * @brief Dequeues the BIOS priority-1 interrupt chain node D_800A3E00 inside a critical section.
 *
 * Wraps a BIOS vector 0xC0 subfunction 3 call between enter/exit critical-section syscalls to unlink D_800A3E00 (the 16-byte node populated by func_8008D964) during teardown. Category BIOS > System Calls | Platform > Interrupts follows the critical-section syscalls and IRQ-chain node D_800A3E00.
 *
 * @param void No parameters.
 * @return Always 1.
 *
 * @par Globals
 * D_800A3E00 - 16-byte BIOS interrupt-chain node passed as the second argument ($a1) to func_8008CA10.
 *
 * @par Control flow
 * Straight-line sequence: enter critical section, dequeue D_800A3E00 at priority 1, exit critical section, return 1.
 *
 * @par Calls
 * func_8008C9A0 - enters BIOS critical section (`syscall 0`, code 1).
 * func_8008CA10 - invokes BIOS vector 0xC0 function 3 with (1, D_800A3E00).
 * func_8008C9B0 - exits BIOS critical section (`syscall 0`, code 2).
 *
 * @par Side effects
 * Masks interrupts briefly and unlinks D_800A3E00 from the PS1 kernel priority-1 IRQ handler chain.
 *
 * @par Callers
 * Confirmed: func_8008D92C (0x8008D944 in asm/us/main.s) / Likely: none / Unknown: none.
 */
int func_8008D9DC(void)
{
    func_8008C9A0();
    {
        register void *base __asm__("$5") = D_800A3E00;
        __asm__ volatile("" : "+r"(base));
        func_8008CA10(1, base);
    }
    func_8008C9B0();
    return 1;
}
