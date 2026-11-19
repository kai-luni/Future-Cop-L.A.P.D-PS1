// Categories: Utilities > String | Platform > BIOS
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS stub: strchr via the A0 vector table, index 0x1E.
 *
 * @param src NUL-terminated string (a0).
 * @param char Character to find (a1).
 * @return Pointer to the first occurrence, or 0 (BIOS return in v0).
 *
 * @par Confirmed
 * Handwritten transcribed stub: addiu $t2, $zero, 0xA0; jr $t2 with
 * addiu $t1, $zero, 0x1E in the delay slot. A(1Eh) is strchr(src, char),
 * documented as identical to index (A(1Ch)) in the PS1 BIOS A0 table
 * (psx-spx / PSXSPX BIOS summary). No local body.
 *
 * @par Likely role
 * The text renderer's character search: func_800788F0 uses it to locate
 * format characters while laying out a string.
 *
 * @par Callers
 * func_800788F0.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x1E to
 * invoke PS1 BIOS A0:1E (strchr), returning a pointer to the first occurrence
 * of $a1 in $a0 or 0. Called by func_800788F0.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCD0\n"
    ".ent func_8008CCD0\n"
    "func_8008CCD0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x1E\n"
    ".end func_8008CCD0\n"
    ".set reorder\n"
);
