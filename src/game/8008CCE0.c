// Categories: BIOS > System Calls | Utilities > String
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS trampoline A0:25 (toupper): converts an ASCII character in $a0 to uppercase.
 *
 * Handwritten MIPS stub executing `addiu $t2, $zero, 0xA0; jr $t2; addiu $t1, $zero, 0x25`, dispatching to PS1 BIOS vector A0 function 0x25 (toupper). Used by func_8001DC48 to normalize file paths to ISO-9660 uppercase. Category BIOS > System Calls | Utilities > String reflects BIOS A0:25 and character case conversion.
 *
 * @param ch (implicit $a0) Input character code.
 * @return (implicit $v0) Uppercase ASCII character code.
 *
 * @par Globals
 * None directly (BIOS vector 0xA0 function 0x25).
 *
 * @par Control flow
 * Straight-line BIOS tail jump setting $t2 = 0xA0 and $t1 = 0x25 in the delay slot.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS A0 vector 0x25).
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8001DC48 (0x8001DDC4 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x25 to
 * invoke PS1 BIOS A0:25 (toupper), converting character $a0 to uppercase.
 * Called by func_8001DC48.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CCE0\n"
    ".ent func_8008CCE0\n"
    "func_8008CCE0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x25\n"
    ".end func_8008CCE0\n"
    ".set reorder\n"
);
