// Categories: Utilities > Output | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS printf() - formatted console output.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x3F, i.e. A(0x3F) = printf.
 *
 * @param (implicit) $a0 format string, further arguments in $a1-$a3 and on the
 *        stack, as usual for a varargs call.
 * @return The number of characters written, as left by the BIOS.
 *
 * @par Callers
 * func_8008DEA0, to report "CdInit: Init failed\n" (D_8001231C) after five
 * failed CD initialisation attempts.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008E000\n"
    ".ent func_8008E000\n"
    "func_8008E000:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x3F\n"
    ".end func_8008E000\n"
    ".set reorder\n"
);
