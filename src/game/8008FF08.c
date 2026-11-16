// Categories: BIOS > System Calls | Utilities > Output
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Invokes PS1 BIOS puts (B0:3F) to write a null-terminated string to console output.
 *
 * Handwritten BIOS vector trampoline setting $t2 = 0xB0 and $t1 = 0x3F. Used by the CD-ROM driver and frame-wait routines to print error, retry, shell-open, and timeout diagnostics.
 *
 * @param (implicit) $a0 pointer to null-terminated string.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line trampoline: load $t2 = 0xB0, jr $t2 with delay-slot $t1 = 0x3F (followed by padding nops).
 *
 * @par Calls
 * None (jumps directly to PS1 BIOS B0 vector).
 *
 * @par Side effects
 * Emits console/TTY output via PS1 BIOS B0:3F.
 *
 * @par Callers
 * Confirmed: func_8008E7A0 (0x8008ECCC), func_8008ECFC (0x8008EDCC), func_8008EF7C (0x8008F04C), func_8008F244 (0x8008F4A8), func_8008F8E8 (0x8008F8F8), func_8008FAC8 (0x8008FB78), func_800908D4 (0x80090998), func_80090C18 (0x80090C8C, 0x80090CD4), func_800913E8 (0x80091434).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008FF08\n"
    ".ent func_8008FF08\n"
    "func_8008FF08:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x3F\n"
    "nop\n"
    "nop\n"
    "nop\n"
    ".end func_8008FF08\n"
    ".set reorder\n"
);
