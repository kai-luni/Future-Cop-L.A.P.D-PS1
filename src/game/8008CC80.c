// Categories: BIOS > System Calls | Platform > CD-ROM
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten PS1 BIOS vector stub calling B0 table entry 0x38 (exit).
 *
 * Confirmed: inline assembly loads $t2 = 0xB0 and jumps to $t2 with $t1 = 0x38 in the delay slot (`jr $t2; addiu $t1, $zero, 0x38` at 0x8008CC80 in asm/us/main.s). BIOS B(0x38) is `exit(exitcode)` taking $a0. Both callers invoke it as `func_8008CC80(-1)` when func_8008E1BC (CD-ROM command 0xE) fails after clearing 0x1F801800/0x1F801803. Category BIOS > System Calls | Platform > CD-ROM follows the B0:0x38 vector dispatch and the CD-ROM error callers.
 * Likely: fatal abort path on unrecoverable CD-ROM setup or directory-read failure.
 * Unknown: none.
 *
 * @param (implicit) $a0 Exit status code forwarded to BIOS B(0x38) (-1 at both call sites).
 *
 * @par Globals
 * None directly referenced.
 *
 * @par Control flow
 * Unconditional tail jump into BIOS B0 dispatch vector 0x38.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS B0:0x38 vector).
 *
 * @par Side effects
 * Transfers control to PS1 BIOS B(0x38) exit handler, terminating execution.
 *
 * @par Callers
 * Confirmed: func_8001C680 (0x8001C6CC, CD-ROM init failure) and func_8001C718 (0x8001C79C, CD-ROM directory-read failure) / Likely: none / Unknown: none.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CC80\n"
    ".ent func_8008CC80\n"
    "func_8008CC80:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x38\n"
    ".end func_8008CC80\n"
    ".set reorder\n"
);
