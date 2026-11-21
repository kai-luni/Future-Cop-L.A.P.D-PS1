// Categories: Graphics > GTE > Batch Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Batch transform of 12-byte vertices, output as three 32-bit values with
 *        Y flipped.
 *
 * @param (implicit) $a0 source array (x, y, z as three words, 12 bytes per
 *        vertex), $a1 destination (12 bytes per vertex), $a2 count (<= 0 skips).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The source words are repacked into the GTE's 16-bit layout: VXY0 =
 * ((x << 12) >> 16) | ((y >> 4) << 16), VZ0 = z >> 4 (both with the mtc2 pair
 * $0/$1), then `mvmva 1,0,0,3,0`; the results are read from IR1..IR3, shifted
 * left by 4 and stored as three words, with Y negated.
 *
 * @par Likely role
 * The 32-bit-precision sibling of func_8001010C for vertices that keep
 * fractional coordinates - 30 call sites, from the object renderers to
 * func_8006DC24 and func_80071FA4.
 *
 * @par Environment path
 * Batch vertex transform with 12-byte 32-bit vertices. Its 30 callers are the
 * map-cell and part drawers (func_80014164, func_80016A64, func_80017058,
 * func_800178DC, func_80031238, func_80035B7C, func_80038A00).
 */
void gGteTransformWords(void) __asm__("func_8001006C");

__asm__(
    ".set noreorder\n"
    ".globl func_8001006C\n"
    ".ent func_8001006C\n"
    "func_8001006C:\n"
    ".word 0x18C00025   /* blez       $a2, .L80010104 */\n"
    ".word 0x8C880000   /* lw        $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x8C8A0008   /* lw         $t2, 0x8($a0) */\n"
    ".word 0x00084300   /* sll        $t0, $t0, 12 */\n"
    ".word 0x00084402   /* srl        $t0, $t0, 16 */\n"
    ".word 0x00094903   /* sra        $t1, $t1, 4 */\n"
    ".word 0x00094C00   /* sll        $t1, $t1, 16 */\n"
    ".word 0x01094025   /* or         $t0, $t0, $t1 */\n"
    ".word 0x000A5103   /* sra        $t2, $t2, 4 */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x488A0800   /* mtc2       $t2, $1 */\n"
    ".word 0x20C6FFFF   /* addi       $a2, $a2, -0x1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A486012   /* mvmva      1, 0, 0, 3, 0 */\n"
    ".word 0x20A5000C   /* addi       $a1, $a1, 0xC */\n"
    ".word 0x2084000C   /* addi       $a0, $a0, 0xC */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x8C8A0008   /* lw         $t2, 0x8($a0) */\n"
    ".word 0x00084300   /* sll        $t0, $t0, 12 */\n"
    ".word 0x00084402   /* srl        $t0, $t0, 16 */\n"
    ".word 0x00094903   /* sra        $t1, $t1, 4 */\n"
    ".word 0x00094C00   /* sll        $t1, $t1, 16 */\n"
    ".word 0x01094025   /* or         $t0, $t0, $t1 */\n"
    ".word 0x000A5103   /* sra        $t2, $t2, 4 */\n"
    ".word 0x480B4800   /* mfc2       $t3, $9 */\n"
    ".word 0x480C5000   /* mfc2       $t4, $10 */\n"
    ".word 0x480D5800   /* mfc2       $t5, $11 */\n"
    ".word 0x000B5900   /* sll        $t3, $t3, 4 */\n"
    ".word 0xACABFFF4   /* sw         $t3, -0xC($a1) */\n"
    ".word 0x000C6022   /* neg        $t4, $t4 */\n"
    ".word 0x000C6100   /* sll        $t4, $t4, 4 */\n"
    ".word 0xACACFFF8   /* sw         $t4, -0x8($a1) */\n"
    ".word 0x000D6900   /* sll        $t5, $t5, 4 */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1020FFE5   /* beqz       $at, .L80010094 */\n"
    ".word 0xACADFFFC   /* sw        $t5, -0x4($a1) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_8001006C\n"
    ".set reorder\n"
);
