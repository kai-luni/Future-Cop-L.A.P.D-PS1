// Categories: BIOS > System Calls | Platform > Interrupts
/* Handwritten function: BIOS vector + setter, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x5B stub that also latches its argument.
 *
 * Besides the vector call the stub stores the argument to D_8009C800 on the way
 * back, so it is both a BIOS thunk and a global setter. The jump layout (two
 * jumps with their delay slots) is reproduced byte for byte from the listing.
 *
 * @param a0 Value that ends up in D_8009C800.
 *
 * @par Globals
 * D_8009C800 - latched argument.
 *
 * @par Control flow
 * Two jumps, one delay slot each.
 *
 * @par Calls
 * None; the BIOS vector does the call.
 *
 * @par Side effects
 * Calls BIOS B0:0x5B and writes D_8009C800.
 *
 * @par Callers
 * Confirmed: the driver block that later reads D_8009C800.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D7B0\n"
    ".ent func_8008D7B0\n"
    "func_8008D7B0:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x5B\n"
    "nop\n"
    "lui       $at, %hi(D_8009C800)\n"
    "jr        $ra\n"
    "sw        $a0, %lo(D_8009C800)($at)\n"
    ".end func_8008D7B0\n"
    ".set reorder\n"
);
