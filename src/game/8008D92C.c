// Categories: BIOS > System Calls | Platform > Interrupts
extern void func_8008DB14(void);
extern void func_8008DAE0(void);
extern void func_8008D9DC(void);
extern int gModeClearFlag __asm__("D_8009C800");

/**
 * @brief Tears down BIOS pad and interrupt-chain hooks and clears initialization flag D_8009C800.
 *
 * Invokes the saved routine via func_8008DB14, stops pad polling via func_8008DAE0 (BIOS B0:14),
 * dequeues IRQ node D_800A3E00 via func_8008D9DC, and resets D_8009C800 to 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009C800 - BIOS/pad initialized status word cleared to 0 at exit.
 *
 * @par Control flow
 * Straight-line sequence of three teardown calls followed by D_8009C800 = 0.
 *
 * @par Calls
 * func_8008DB14 - jumps to jtbl_800A3E1C_main; func_8008DAE0 - BIOS StopPAD (B0:14); func_8008D9DC - dequeues D_800A3E00.
 *
 * @par Side effects
 * Stops BIOS pad service, removes priority-1 IRQ handler D_800A3E00, and clears D_8009C800.
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s / Likely: unreferenced BIOS/pad teardown counterpart to func_8008D7DC / Unknown: whether invoked indirectly.
 */
void func_8008D92C(void)
{
    func_8008DB14();
    func_8008DAE0();
    func_8008D9DC();
    gModeClearFlag = 0;
}
