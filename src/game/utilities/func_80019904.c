// Categories: Utilities > Mathematics > Fixed Point | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Table-driven normalisation: scales a value into the range of a
 *        small-data lookup table via the GTE's leading-zero counter.
 *
 * @param (implicit) $a0 the value to normalise.
 * @return In $v0: the table value scaled back by the same factor.
 *
 * @par Confirmed mechanics
 * mtc2 $a0,$30 latches the value into LZCS and mfc2 $t2,$31 reads LZCR, the
 * leading-zero/one count. From it the function derives
 * exp = 0x16 - (LZCR & ~1), clamps it at 0, shifts the input right by exp,
 * halves exp and doubles the shifted value to build a word index, loads a
 * half-word from the table at $gp + 0x39C, and returns
 * ((entry >> 5) << (entry & 0x1F)) >> (exp / 2) - a fused "look up, then shift
 * back" that keeps the result in the same fixed-point scale as the input.
 *
 * @par Likely role
 * A reciprocal or square-root table applied to a magnitude (used by
 * func_80016C88, func_8001A180 and func_8005F018). The exact table content is
 * in small data at 0x8009Fxxx; not decoded here.
 *
 * @par Unknown
 * The semantic of the table entries (Likely 1/x or 1/sqrt(x) in 5.11 fixed
 * point, judging from the two-field packing).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80019904\n"
    ".ent func_80019904\n"
    "func_80019904:\n"
    ".word 0x27BDFFF8   /* addiu      $sp, $sp, -0x8 */\n"
    ".word 0xAFBF0000   /* sw         $ra, 0x0($sp) */\n"
    ".word 0x4884F000   /* mtc2       $a0, $30 */\n"
    ".word 0x2408FFFE   /* addiu      $t0, $zero, -0x2 */\n"
    ".word 0x24090016   /* addiu      $t1, $zero, 0x16 */\n"
    ".word 0x480AF800   /* mfc2       $t2, $31 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x01485024   /* and        $t2, $t2, $t0 */\n"
    ".word 0x012A5022   /* sub        $t2, $t1, $t2 */\n"
    ".word 0x1D400002   /* bgtz       $t2, .L80019934 */\n"
    ".word 0x8F8B039C   /* lw        $t3, 0x39C($gp) */\n"
    ".word 0x240A0000   /* addiu      $t2, $zero, 0x0 */\n"
    ".word 0x01442007   /* srav       $a0, $a0, $t2 */\n"
    ".word 0x000A5043   /* sra        $t2, $t2, 1 */\n"
    ".word 0x00042040   /* sll        $a0, $a0, 1 */\n"
    ".word 0x01645820   /* add        $t3, $t3, $a0 */\n"
    ".word 0x95640000   /* lhu        $a0, 0x0($t3) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x3089001F   /* andi       $t1, $a0, 0x1F */\n"
    ".word 0x00042143   /* sra        $a0, $a0, 5 */\n"
    ".word 0x01242004   /* sllv       $a0, $a0, $t1 */\n"
    ".word 0x01441007   /* srav       $v0, $a0, $t2 */\n"
    ".word 0x8FBF0000   /* lw         $ra, 0x0($sp) */\n"
    ".word 0x27BD0008   /* addiu      $sp, $sp, 0x8 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80019904\n"
    ".set reorder\n"
);
