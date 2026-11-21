// Categories: BIOS > System Calls | Platform > Memory Card
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS trampoline A0:70 (_bu_init): initializes the PS1 memory card / backup unit BIOS subsystem.
 *
 * Handwritten MIPS stub executing `addiu $t2, $zero, 0xA0; jr $t2; addiu $t1, $zero, 0x70`, dispatching to PS1 BIOS vector A0 function 0x70 (_bu_init) after CARD setup in gInitMemoryCardBios. Category BIOS > System Calls | Platform > Memory Card follows BIOS vector A0:70 and the surrounding CARD init sequence.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly (BIOS vector 0xA0 function 0x70).
 *
 * @par Control flow
 * Straight-line BIOS tail jump setting $t2 = 0xA0 and $t1 = 0x70 in the delay slot.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS A0 vector 0x70).
 *
 * @par Side effects
 * Initializes BIOS memory card / backup unit state via BIOS A0:70.
 *
 * @par Callers
 * Confirmed: gInitMemoryCardBios (0x800200D0 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x70 to
 * invoke PS1 BIOS A0:70 (_bu_init), initializing the memory card / backup
 * unit subsystem. Called by gInitMemoryCardBios.
 */
void gInitMemoryCardBiosUnit(void) __asm__("func_8008C950");

__asm__(
    ".set noreorder\n"
    ".globl func_8008C950\n"
    ".ent func_8008C950\n"
    "func_8008C950:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x70\n"
    ".end func_8008C950\n"
    ".set reorder\n"
);
