// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS strcmp() - compares two NUL-terminated strings.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x17, i.e. A(0x17) = strcmp.
 *
 * @param (implicit) $a0, $a1 the two strings.
 * @return 0 when equal, otherwise the difference of the first differing
 *         characters (negative/positive as returned by the BIOS).
 *
 * @par Callers
 * func_80083130 and func_800833A8 (module-name lookups, always compared against
 * 0), func_80020C90, func_8007B854 and func_800904DC.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x17 to
 * invoke PS1 BIOS A0:17 (strcmp), comparing strings $a0 and $a1. Called by
 * func_80083130, func_800833A8, func_80020C90, func_8007B854, and
 * func_800904DC.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCA0\n"
    ".ent func_8008CCA0\n"
    "func_8008CCA0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x17\n"
    ".end func_8008CCA0\n"
    ".set reorder\n"
);
