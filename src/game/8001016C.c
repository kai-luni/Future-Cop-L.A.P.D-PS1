// Categories: Graphics > GTE > Vector Batch Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Batch vertex transform with an extra Y correction of 15/16.
 *
 * @param (implicit) $a0 source array, $a1 destination array, $a2 count.
 * @return Nothing.
 *
 * @par Confirmed
 * Identical to func_8001010C (`mvmva 1,0,0,3,0`, Y negated, three `sh`), plus
 * two instructions on Y: `sra $t2,$t3,4` and `sub $t3,$t3,$t2`, i.e.
 * y = y - y/16 = y * 15/16.
 *
 * @par Likely role
 * The same transform for geometry that has to be squashed slightly on Y -
 * a cheap aspect-ratio correction for the 320x240 screen.
 *
 * @par Callers
 * None in the symbol table (inlined); the callers of the sibling functions use
 * whichever variant their projection needs.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8001016C\n"
    ".ent func_8001016C\n"
    "func_8001016C:\n"
    ".word 0x18C00017   /* blez       $a2, .L800101CC */\n"
    ".word 0x8C880000   /* lw        $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A486012   /* mvmva      1, 0, 0, 3, 0 */\n"
    ".word 0x20C6FFFF   /* addi       $a2, $a2, -0x1 */\n"
    ".word 0x20A50008   /* addi       $a1, $a1, 0x8 */\n"
    ".word 0x20840008   /* addi       $a0, $a0, 0x8 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x480A4800   /* mfc2       $t2, $9 */\n"
    ".word 0x480B5000   /* mfc2       $t3, $10 */\n"
    ".word 0x480C5800   /* mfc2       $t4, $11 */\n"
    ".word 0xA4AAFFF8   /* sh         $t2, -0x8($a1) */\n"
    ".word 0x000B5822   /* neg        $t3, $t3 */\n"
    ".word 0x000B5103   /* sra        $t2, $t3, 4 */\n"
    ".word 0x016A5822   /* sub        $t3, $t3, $t2 */\n"
    ".word 0xA4ABFFFA   /* sh         $t3, -0x6($a1) */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1020FFEC   /* beqz       $at, .L80010178 */\n"
    ".word 0xA4ACFFFC   /* sh        $t4, -0x4($a1) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_8001016C\n"
    ".set reorder\n"
);
