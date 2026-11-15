// Categories: Graphics > GTE > Projection | Graphics > Clipping
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Projects a vertex list three at a time (RTPT) and returns only the
 *        screen positions plus the accumulated flag.
 *
 * @param (implicit) $a0 source vertices, $a1 output (three words: SXY0, SXY1,
 *        SXY2 per group, 12 bytes), $a2 count, $a3 unused.
 * @return In $v0: the OR of the FLAG register after every command.
 *
 * @par Confirmed structure
 * Main loop while count >= 3: load six words into VXY0/VZ0/VXY1/VZ1/VXY2/VZ2
 * ($0..$5), `rtpt`, advance 0x18/0xC, then swc2 $12/$13/$14 store the three
 * screen positions. The tail (1 or 2 vertices) runs one `rtps` per remaining
 * vertex and stores SXY2 at a 4-byte stride.
 *
 * @par Compare
 * func_8001054C writes SXY and SZ through swc2 as well, func_800102A0 adds the
 * per-vertex clip byte.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80010630\n"
    ".ent func_80010630\n"
    "func_80010630:\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x8C8A0008   /* lw         $t2, 0x8($a0) */\n"
    ".word 0x8C8B000C   /* lw         $t3, 0xC($a0) */\n"
    ".word 0x8C8C0010   /* lw         $t4, 0x10($a0) */\n"
    ".word 0x8C8D0014   /* lw         $t5, 0x14($a0) */\n"
    ".word 0x00001021   /* addu       $v0, $zero, $zero */\n"
    ".word 0x28C10003   /* slti       $at, $a2, 0x3 */\n"
    ".word 0x1420001A   /* bnez       $at, .L800106BC */\n"
    ".word 0x28C10001   /* slti      $at, $a2, 0x1 */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x488A1000   /* mtc2       $t2, $2 */\n"
    ".word 0x488B1800   /* mtc2       $t3, $3 */\n"
    ".word 0x488C2000   /* mtc2       $t4, $4 */\n"
    ".word 0x488D2800   /* mtc2       $t5, $5 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A280030   /* rtpt */\n"
    ".word 0x20840018   /* addi       $a0, $a0, 0x18 */\n"
    ".word 0x20A5000C   /* addi       $a1, $a1, 0xC */\n"
    ".word 0x20C6FFFD   /* addi       $a2, $a2, -0x3 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x8C8A0008   /* lw         $t2, 0x8($a0) */\n"
    ".word 0x8C8B000C   /* lw         $t3, 0xC($a0) */\n"
    ".word 0x8C8C0010   /* lw         $t4, 0x10($a0) */\n"
    ".word 0x8C8D0014   /* lw         $t5, 0x14($a0) */\n"
    ".word 0xE8ACFFF4   /* swc2       $12, -0xC($a1) */\n"
    ".word 0xE8ADFFF8   /* swc2       $13, -0x8($a1) */\n"
    ".word 0xE8AEFFFC   /* swc2       $14, -0x4($a1) */\n"
    ".word 0x4843F800   /* cfc2       $v1, $31 */\n"
    ".word 0x0401FFE6   /* bgez       $zero, .L8001064C */\n"
    ".word 0x00431025   /* or        $v0, $v0, $v1 */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1420000F   /* bnez       $at, .L800106FC */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A180001   /* rtps */\n"
    ".word 0x20C6FFFF   /* addi       $a2, $a2, -0x1 */\n"
    ".word 0x20A50004   /* addi       $a1, $a1, 0x4 */\n"
    ".word 0x20840008   /* addi       $a0, $a0, 0x8 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0xE8AEFFFC   /* swc2       $14, -0x4($a1) */\n"
    ".word 0x4843F800   /* cfc2       $v1, $31 */\n"
    ".word 0x0401FFF0   /* bgez       $zero, .L800106B8 */\n"
    ".word 0x00431025   /* or        $v0, $v0, $v1 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80010630\n"
    ".set reorder\n"
);
