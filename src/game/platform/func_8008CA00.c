// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS C0:0x02 stub.
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the C0 vector.
 * @par Calls
 * None; the BIOS routine runs.
 * @par Side effects
 * Whatever the C0 routine performs.
 * @par Callers
 * Confirmed: the interrupt/event block of the CD driver.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xC0 and $t1 = 0x02 to
 * invoke PS1 BIOS C0:02 (SysEnqIntRP), enqueuing an interrupt handler node on
 * a BIOS priority list. Called by func_8008A5BC and the CD/interrupt setup
 * path.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CA00\n"
    ".ent func_8008CA00\n"
    "func_8008CA00:\n"
    "addiu     $t2, $zero, 0xC0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2\n"
    ".end func_8008CA00\n"
    ".set reorder\n"
);
