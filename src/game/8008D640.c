// Categories: BIOS > System Calls | Platform > Memory Card
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief BIOS trampoline A0:AB (_card_info): queries memory card status for the given channel.
 *
 * Handwritten MIPS stub executing `addiu $t2, $zero, 0xA0; jr $t2; addiu $t1, $zero, 0xAB`, dispatching to PS1 BIOS vector A0 function 0xAB (_card_info) across the D_8009AB08 memory-card state machine. Category BIOS > System Calls | Platform > Memory Card reflects BIOS A0:AB and card-channel callers.
 *
 * @param chan (implicit $a0) Memory card channel handle (e.g. D_8009AB08->unk8FC).
 * @return (implicit $v0) BIOS A0:AB return code.
 *
 * @par Globals
 * None directly (BIOS vector 0xA0 function 0xAB).
 *
 * @par Control flow
 * Straight-line BIOS tail jump setting $t2 = 0xA0 and $t1 = 0xAB in the delay slot.
 *
 * @par Calls
 * None (tail-jumps to PS1 BIOS A0 vector 0xAB).
 *
 * @par Side effects
 * Initiates/queries memory-card info via BIOS A0:AB.
 *
 * @par Callers
 * Confirmed: func_8001F394, func_8001F784, func_8001F818, func_8001F968, func_8001FAB8, func_8001FD28, func_8001FE40, func_8001FF44, func_80020024, func_800206D0 / Likely: none / Unknown: none.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D640\n"
    ".ent func_8008D640\n"
    "func_8008D640:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xAB\n"
    ".end func_8008D640\n"
    ".set reorder\n"
);
