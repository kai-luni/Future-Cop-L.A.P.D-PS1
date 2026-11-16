// Categories: Game Engine > State
/* Handwritten function: retail body is three loads off $gp with no return. */
/**
 *  @brief Loads two GP-relative bytes into registers.
 *
 *  This assembler fragment reads the byte at offset 0x660 as signed and
 *  the byte at offset 0x661 as both signed and unsigned.
 *  @par Globals
 *  No named globals are identified; the reads use GP-relative addresses.
 *  @par Control flow
 *  Three loads execute sequentially, with no branch or return instruction.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the loaded values to registers $2, $4, and $3.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800826A0\n"
    ".ent func_800826A0\n"
    "func_800826A0:\n"
    "lb      $2, 0x660($gp)\n"
    "lb      $4, 0x661($gp)\n"
    "lbu     $3, 0x661($gp)\n"
    ".end func_800826A0\n"
    ".set reorder\n"
);
