// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS strcpy() - copies a string including its terminator.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x19, i.e. A(0x19) =
 * strcpy(dst, src).
 *
 * @param (implicit) $a0 destination buffer, @c $a1 source string.
 * @return The incoming destination pointer, as left by the BIOS.
 *
 * @par Callers
 * func_80076A2C (copying a pending module name before appending ".mis"),
 * func_80024904 / func_80024A24 (storing the current module path), func_8001DC48,
 * func_8007799C, func_8007B854, func_80080B70, func_8008125C and the unit at
 * 0x80078C8C - it is the single most used string helper in the game.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCB0\n"
    ".ent func_8008CCB0\n"
    "func_8008CCB0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x19\n"
    ".end func_8008CCB0\n"
    ".set reorder\n"
);
