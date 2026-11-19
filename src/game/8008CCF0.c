// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS tolower() - converts one character to lower case.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x26, i.e. A(0x26) = tolower.
 *
 * @param (implicit) $a0 the character.
 * @return The lower-case form ('A'-'Z' mapped, everything else unchanged).
 *
 * @par Callers
 * func_80083130 lower-cases the module name before the table lookup, so
 * "FE.mis", "fe.mis" and "Fe.Mis" all resolve to the same entry.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x26 to
 * invoke PS1 BIOS A0:26 (tolower), converting character $a0 to lowercase.
 * Called by func_80083130.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCF0\n"
    ".ent func_8008CCF0\n"
    "func_8008CCF0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x26\n"
    ".end func_8008CCF0\n"
    ".set reorder\n"
);
