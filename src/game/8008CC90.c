// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS strcat() - appends a string to another in place.
 *
 * Handwritten BIOS vector call: $t2 = 0xA0, $t1 = 0x15, i.e. A(0x15), which is
 * strcat(dst, src) in the Sony A0 table. Both arguments are passed through in
 * $a0/$a1 untouched and the return value is whatever the BIOS leaves behind.
 *
 * @param (implicit) $a0 destination string, must be NUL terminated and large
 *        enough; @c $a1 source string.
 * @return The BIOS return value: the incoming destination pointer.
 *
 * @par Callers
 * func_80076A2C appends ".mis" to the module name in D_8009B300, and
 * func_8001DC48.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CC90\n"
    ".ent func_8008CC90\n"
    "func_8008CC90:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x15\n"
    ".end func_8008CC90\n"
    ".set reorder\n"
);
