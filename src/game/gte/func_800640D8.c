// Categories: Graphics > GTE > Matrix Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Transforms one vertex by an explicit matrix and stores it as three
 *        half-words with the Y axis flipped.
 *
 * @param (implicit) $a0 pointer to the packed source vertex (VXY0, VZ0),
 *        $a1 destination (three s16).
 * @return In $v0: the GTE FLAG register.
 *
 * @par Confirmed mechanics
 * lwc2 $0/$1, two nops, `0x4A486012` = MVMVA (real opcode 12h) with sf = 1,
 * mx = 0 (rotation matrix), v = 0 (V0), cv = 0, then the three results are
 * read back from IR1..IR3 with mfc2 and stored with `sh`; the Y component is
 * negated first (`neg $t1,$t1`).
 *
 * @par Likely role
 * The "model to world" step of a small object path where the results are kept
 * as 16-bit coordinates - the Y negation converts between the two screen/axis
 * conventions used by the engine.
 *
 * @par Callers
 * func_8006411C and func_8006429C.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800640D8\n"
    ".ent func_800640D8\n"
    "func_800640D8:\n"
    ".word 0xC8800000   /* lwc2       $0, 0x0($a0) */\n"
    ".word 0xC8810004   /* lwc2       $1, 0x4($a0) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A486012   /* mvmva      1, 0, 0, 3, 0 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x48084800   /* mfc2       $t0, $9 */\n"
    ".word 0x48095000   /* mfc2       $t1, $10 */\n"
    ".word 0x480A5800   /* mfc2       $t2, $11 */\n"
    ".word 0xA4A80000   /* sh         $t0, 0x0($a1) */\n"
    ".word 0x00094822   /* neg        $t1, $t1 */\n"
    ".word 0xA4A90002   /* sh         $t1, 0x2($a1) */\n"
    ".word 0xA4AA0004   /* sh         $t2, 0x4($a1) */\n"
    ".word 0x4842F800   /* cfc2       $v0, $31 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_800640D8\n"
    ".set reorder\n"
);
