// Categories: Platform > BIOS | Utilities > Memory
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS `memcpy` through the A0 vector (A(0x2A)).
 *
 * @param (implicit) $a0 destination, $a1 source, $a2 byte count - the BIOS call
 *        convention, not a C signature.
 * @return Nothing; the BIOS returns the destination in $v0.
 *
 * @par Confirmed
 * Handwritten: `addiu $t2, $zero, 0xA0` / `jr $t2` / `addiu $t1, $zero, 0x2A`,
 * i.e. a jump into the BIOS vector table entry 0x2A. The callers pass the three
 * arguments in $a0/$a1/$a2 (see func_8005A414).
 *
 * @par Why handwritten
 * No C form reproduces the `jr $t2` dispatch, so the words are transcribed 1:1.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x2A to
 * invoke PS1 BIOS A0:2A (memcpy), copying $a2 bytes from $a1 to $a0 and
 * returning $a0. Called by func_8005A414.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CD10\n"
    ".ent func_8008CD10\n"
    "func_8008CD10:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2A\n"
    ".end func_8008CD10\n"
    ".set reorder\n"
);
