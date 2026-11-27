// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS A0:0x18 stub (a single NOP-padded vector call).
 * @par Globals
 * None.
 * @par Control flow
 * Tail jump with one padding NOP.
 * @par Calls
 * None; BIOS A0:0x18 runs.
 * @par Side effects
 * Whatever the BIOS routine performs.
 * @par Callers
 * Confirmed: the CD file service block (func_800908D4 neighbourhood).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80090890\n"
    ".ent func_80090890\n"
    "func_80090890:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x18\n"
    "nop\n"
    ".end func_80090890\n"
    ".set reorder\n"
);
