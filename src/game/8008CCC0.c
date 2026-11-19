// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS strlen() - length of a NUL-terminated string.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x1B, i.e. A(0x1B) = strlen.
 *
 * @param (implicit) $a0 the string.
 * @return The number of characters before the terminator.
 *
 * @par Callers
 * func_80083130 (clamping the name scan), func_8001DC48, func_80020F34,
 * func_80025F58, func_80059BFC, func_8007799C, func_800788F0, func_8007A9D0,
 * func_8008125C, func_800822CC and the formatter body at 0x8008CD3C.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x1B to
 * invoke PS1 BIOS A0:1B (strlen), returning the length of string $a0. Called
 * by func_80083130, func_8008CD3C, and other string callers.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCC0\n"
    ".ent func_8008CCC0\n"
    "func_8008CCC0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x1B\n"
    ".end func_8008CCC0\n"
    ".set reorder\n"
);
