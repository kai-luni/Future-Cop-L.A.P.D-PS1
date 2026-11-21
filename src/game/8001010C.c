// Categories: Graphics > GTE > Vector Batch Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Transforms an array of vertices with the current rotation matrix and
 *        stores them as three half-words each (Y negated).
 *
 * @param (implicit) $a0 source array, 8 bytes per vertex (VXY0 at +0, VZ0 at +4).
 * @param (implicit) $a1 destination array, 8 bytes per vertex (three s16 plus
 *        padding).
 * @param (implicit) $a2 vertex count; <= 0 skips the loop entirely.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Per vertex: two lw, two mtc2 ($0, $1), two nops, `mvmva 1,0,0,3,0` (sf = 1,
 * rotation matrix, IR vector as input), then mfc2 $9/$10/$11 (IR1..IR3) and
 * three `sh` stores; the Y component is negated first. Pointers advance by 8
 * and the count is decremented before the loop test, so the last store lands in
 * the delay slot of the branch - the classic MIPS batch shape.
 *
 * @par Likely role
 * The bulk "model to view" transform used by the object renderers: three
 * 16-bit coordinates are enough for the downstream projection, and the Y
 * negation converts the model's Y-up axis to the screen's Y-down convention.
 *
 * @par Callers
 * func_80013F08, func_8001428C, func_80016C88, func_8004A048, func_80051CC4,
 * func_800597C0, func_80070158.
 *
 * @par Siblings
 * func_800101D4 (vector register as input), func_8001016C (adds a 15/16
 * correction on Y), func_80010234 (32-bit results, output stride 0xC).
 *
 * @par Environment path
 * Batch vertex transform of 8-byte vertices. Called by the light setups
 * (func_80013F08, func_8001428C), the map-cell drawers (func_8004A048,
 * func_80051CC4) and the player instance submission (func_800597C0).
 */
void gGteTransformVertices(void) __asm__("func_8001010C");

__asm__(
    ".set noreorder\n"
    ".globl func_8001010C\n"
    ".ent func_8001010C\n"
    "func_8001010C:\n"
    ".word 0x18C00015   /* blez       $a2, .L80010164 */\n"
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
    ".word 0xA4ABFFFA   /* sh         $t3, -0x6($a1) */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1020FFEE   /* beqz       $at, .L80010118 */\n"
    ".word 0xA4ACFFFC   /* sh        $t4, -0x4($a1) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_8001010C\n"
    ".set reorder\n"
);
