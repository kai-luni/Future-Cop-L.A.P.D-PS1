// Categories: Graphics > GTE > Vector Batch Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Batch vertex transform that feeds the vertex into V0 instead of IR.
 *
 * @param (implicit) $a0 source array (8 bytes per vertex), $a1 destination
 *        (8 bytes per vertex), $a2 count.
 * @return Nothing.
 *
 * @par Confirmed
 * Same loop as func_8001010C but the command is `mvmva 1,0,0,0,0`: v = 0, so the
 * multiply uses the V0 register pair that was just loaded with mtc2 $0/$1,
 * and the flag lm = 0 (no limit). Results are again read from IR1..IR3, Y is
 * negated, three `sh` stores.
 *
 * @par Callers
 * func_80013F08 and func_800592D0.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800101D4\n"
    ".ent func_800101D4\n"
    "func_800101D4:\n"
    ".word 0x18C00015   /* blez       $a2, .L8001022C */\n"
    ".word 0x8C880000   /* lw        $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A480012   /* mvmva      1, 0, 0, 0, 0 */\n"
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
    ".word 0xA4ABFFFA   /* sh         $t3, -0x6($a1) */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1020FFEE   /* beqz       $at, .L800101E0 */\n"
    ".word 0xA4ACFFFC   /* sh        $t4, -0x4($a1) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_800101D4\n"
    ".set reorder\n"
);
