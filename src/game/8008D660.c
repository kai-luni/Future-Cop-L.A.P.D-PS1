// Categories: BIOS > System Calls | Platform > Memory Card
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS trampoline A0:AD: invokes PS1 BIOS vector A0 function 0xAD with $a0.
 *
 * Handwritten MIPS stub executing `addiu $t2, $zero, 0xA0; jr $t2; addiu $t1, $zero, 0xAD`, called with 0 inside the critical-section card initialization sequence in gInitMemoryCardBios between InitCARD (func_8008D6E0) and StartCARD (func_8008D74C). Category BIOS > System Calls | Platform > Memory Card reflects BIOS A0:AD and card init caller.
 *
 * @param arg0 (implicit $a0) Integer argument passed by caller (0 in gInitMemoryCardBios).
 * @return (implicit $v0) BIOS A0:AD return value.
 *
 * @par Globals
 * None directly (BIOS vector 0xA0 function 0xAD).
 *
 * @par Control flow
 * Straight-line BIOS tail jump setting $t2 = 0xA0 and $t1 = 0xAD in the delay slot.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS A0 vector 0xAD).
 *
 * @par Side effects
 * Configures BIOS memory card state via BIOS A0:AD (likely _card_auto).
 *
 * @par Callers
 * Confirmed: gInitMemoryCardBios (0x800200C0 in asm/us/main.s) / Likely: none / Unknown: none.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D660\n"
    ".ent func_8008D660\n"
    "func_8008D660:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xAD\n"
    ".end func_8008D660\n"
    ".set reorder\n"
);
