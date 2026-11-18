// Categories: Graphics > Model Renderer > Textured Surfaces | Graphics > Clipping
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Builds the triangle strips of a textured surface with per-strip clipping.
 *
 * @param (implicit) $a0 vertex list, $a1 destination packet run, $a2 rectangle
 *        (x, y, w as half-words), $a3 bit mask selecting the texture page.
 * @return Nothing.
 *
 * @par Confirmed structure
 * The screen width is preloaded from D_8009FA0C (0x140 or 0xA0) and used to clamp
 * each strip. The loop walks the vertex list in 0x4C-byte steps (0x33 alternately,
 * i.e. two interleaved strides), packs the 4-bit attribute of each vertex into a
 * running word (`sllv $a3, ($s4 & ~3)`, masked with 0xF0000000) and, as long as
 * that word is zero, skips 0x33 bytes and 0x198 bytes in the destination - the
 * "no attributes in this column" fast path. Otherwise it emits a strip of up to
 * 0x11 vertices and copies the gathered attributes.
 *
 * @par Likely role
 * The surface rasteriser of the level renderer: attributes are 4-bit indices that
 * select a palette row per vertex, and the odd strides come from the two
 * interleaved vertex arrays.
 *
 * @par Unknown
 * The meaning of 0x33/0x198 (likely 51-byte vertex records and 408-byte packet
 * groups).
 *
 * @par Environment path
 * Emits the textured triangle strips of one region surface with per-strip
 * clipping; the tile grid calls it once per populated region.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800646BC\n"
    ".ent func_800646BC\n"
    "func_800646BC:\n"
    ".word 0x27BDFFE0   /* addiu      $sp, $sp, -0x20 */\n"
    ".word 0xAFB20000   /* sw         $s2, 0x0($sp) */\n"
    ".word 0xAFB30004   /* sw         $s3, 0x4($sp) */\n"
    ".word 0xAFB40008   /* sw         $s4, 0x8($sp) */\n"
    ".word 0xAFB5000C   /* sw         $s5, 0xC($sp) */\n"
    ".word 0xAFB60010   /* sw         $s6, 0x10($sp) */\n"
    ".word 0xAFB00014   /* sw         $s0, 0x14($sp) */\n"
    ".word 0xAFBF0018   /* sw         $ra, 0x18($sp) */\n"
    ".word 0xAFB1001C   /* sw         $s1, 0x1C($sp) */\n"
    ".word 0x3C0A800A   /* lui        $t2, %hi(D_8009FA0C) */\n"
    ".word 0x8D4AFA0C   /* lw         $t2, %lo(D_8009FA0C)($t2) */\n"
    ".word 0x24090001   /* addiu      $t1, $zero, 0x1 */\n"
    ".word 0x112A0002   /* beq        $t1, $t2, .L800646F8 */\n"
    ".word 0x24110140   /* addiu     $s1, $zero, 0x140 */\n"
    ".word 0x241100A0   /* addiu      $s1, $zero, 0xA0 */\n"
    ".word 0x84C80000   /* lh         $t0, 0x0($a2) */\n"
    ".word 0x84C90002   /* lh         $t1, 0x2($a2) */\n"
    ".word 0x84CA0004   /* lh         $t2, 0x4($a2) */\n"
    ".word 0x208D000C   /* addi       $t5, $a0, 0xC */\n"
    ".word 0x00A07021   /* addu       $t6, $a1, $zero */\n"
    ".word 0x01405821   /* addu       $t3, $t2, $zero */\n"
    ".word 0x240C0000   /* addiu      $t4, $zero, 0x0 */\n"
    ".word 0x24140000   /* addiu      $s4, $zero, 0x0 */\n"
    ".word 0x3C060001   /* lui        $a2, (0x10000 >> 16) */\n"
    ".word 0x2415FF80   /* addiu      $s5, $zero, -0x80 */\n"
    ".word 0x24160004   /* addiu      $s6, $zero, 0x4 */\n"
    ".word 0x2401FFFC   /* addiu      $at, $zero, -0x4 */\n"
    ".word 0x02810824   /* and        $at, $s4, $at */\n"
    ".word 0x00271004   /* sllv       $v0, $a3, $at */\n"
    ".word 0x3C01F000   /* lui        $at, (0xF0000000 >> 16) */\n"
    ".word 0x00221024   /* and        $v0, $at, $v0 */\n"
    ".word 0x004C9825   /* or         $s3, $v0, $t4 */\n"
    ".word 0x16600004   /* bnez       $s3, .L80064750 */\n"
    ".word 0x00406021   /* addu      $t4, $v0, $zero */\n"
    ".word 0x25AD0033   /* addiu      $t5, $t5, 0x33 */\n"
    ".word 0x0401004C   /* bgez       $zero, .L8006487C */\n"
    ".word 0x25CE0198   /* addiu     $t6, $t6, 0x198 */\n"
    ".word 0x240F0000   /* addiu      $t7, $zero, 0x0 */\n"
    ".word 0x24100011   /* addiu      $s0, $zero, 0x11 */\n"
    ".word 0x06610004   /* bgez       $s3, .L8006476C */\n"
    ".word 0x01E0F821   /* addu      $ra, $t7, $zero */\n"
    ".word 0x241F0004   /* addiu      $ra, $zero, 0x4 */\n"
    ".word 0x04010002   /* bgez       $zero, .L80064770 */\n"
    ".word 0x240F0001   /* addiu     $t7, $zero, 0x1 */\n"
    ".word 0x240F0000   /* addiu      $t7, $zero, 0x0 */\n"
    ".word 0x00139840   /* sll        $s3, $s3, 1 */\n"
    ".word 0x03E02821   /* addu       $a1, $ra, $zero */\n"
    ".word 0x18A0002F   /* blez       $a1, .L80064838 */\n"
    ".word 0x20A5FFFF   /* addi      $a1, $a1, -0x1 */\n"
    ".word 0x24120002   /* addiu      $s2, $zero, 0x2 */\n"
    ".word 0x81A20000   /* lb         $v0, 0x0($t5) */\n"
    ".word 0xA1C60007   /* sb         $a2, 0x7($t6) */\n"
    ".word 0x3101FFFF   /* andi       $at, $t0, 0xFFFF */\n"
    ".word 0x10550022   /* beq        $v0, $s5, .L8006481C */\n"
    ".word 0x25AD0001   /* addiu     $t5, $t5, 0x1 */\n"
    ".word 0x00021100   /* sll        $v0, $v0, 4 */\n"
    ".word 0x01221020   /* add        $v0, $t1, $v0 */\n"
    ".word 0x00021400   /* sll        $v0, $v0, 16 */\n"
    ".word 0x00411025   /* or         $v0, $v0, $at */\n"
    ".word 0x48820000   /* mtc2       $v0, $0 */\n"
    ".word 0x488A0800   /* mtc2       $t2, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A180001   /* rtps */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x48027000   /* mfc2       $v0, $14 */\n"
    ".word 0x48049800   /* mfc2       $a0, $19 */\n"
    ".word 0xADC20000   /* sw         $v0, 0x0($t6) */\n"
    ".word 0xA5C40004   /* sh         $a0, 0x4($t6) */\n"
    ".word 0x28810601   /* slti       $at, $a0, 0x601 */\n"
    ".word 0x14200010   /* bnez       $at, .L8006481C */\n"
    ".word 0x3043FFFF   /* andi      $v1, $v0, 0xFFFF */\n"
    ".word 0x00021402   /* srl        $v0, $v0, 16 */\n"
    ".word 0x04600005   /* bltz       $v1, .L800647FC */\n"
    ".word 0x24040034   /* addiu     $a0, $zero, 0x34 */\n"
    ".word 0x0071082A   /* slt        $at, $v1, $s1 */\n"
    ".word 0x10200002   /* beqz       $at, .L800647FC */\n"
    ".word 0x24040038   /* addiu     $a0, $zero, 0x38 */\n"
    ".word 0x24040032   /* addiu      $a0, $zero, 0x32 */\n"
    ".word 0x04400005   /* bltz       $v0, .L80064814 */\n"
    ".word 0x2403001C   /* addiu     $v1, $zero, 0x1C */\n"
    ".word 0x284100F0   /* slti       $at, $v0, 0xF0 */\n"
    ".word 0x10200002   /* beqz       $at, .L80064814 */\n"
    ".word 0x2403002C   /* addiu     $v1, $zero, 0x2C */\n"
    ".word 0x2403000E   /* addiu      $v1, $zero, 0xE */\n"
    ".word 0x00642024   /* and        $a0, $v1, $a0 */\n"
    ".word 0xA1C40007   /* sb         $a0, 0x7($t6) */\n"
    ".word 0x2252FFFF   /* addi       $s2, $s2, -0x1 */\n"
    ".word 0x0641FFD8   /* bgez       $s2, .L80064784 */\n"
    ".word 0x25CE0008   /* addiu     $t6, $t6, 0x8 */\n"
    ".word 0x12000014   /* beqz       $s0, .L8006487C */\n"
    ".word 0x2610FFFF   /* addiu     $s0, $s0, -0x1 */\n"
    ".word 0x1CA0FFD2   /* bgtz       $a1, .L8006477C */\n"
    ".word 0x214A0200   /* addi      $t2, $t2, 0x200 */\n"
    ".word 0x02DFF822   /* sub        $ra, $s6, $ra */\n"
    ".word 0x13E0FFC6   /* beqz       $ra, .L80064758 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x12600003   /* beqz       $s3, .L80064854 */\n"
    ".word 0x021F082A   /* slt       $at, $s0, $ra */\n"
    ".word 0x10200002   /* beqz       $at, .L80064858 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x0200F821   /* addu       $ra, $s0, $zero */\n"
    ".word 0x001F0840   /* sll        $at, $ra, 1 */\n"
    ".word 0x003F0820   /* add        $at, $at, $ra */\n"
    ".word 0x01A16820   /* add        $t5, $t5, $at */\n"
    ".word 0x000108C0   /* sll        $at, $at, 3 */\n"
    ".word 0x01C17020   /* add        $t6, $t6, $at */\n"
    ".word 0x021F8022   /* sub        $s0, $s0, $ra */\n"
    ".word 0x001F1240   /* sll        $v0, $ra, 9 */\n"
    ".word 0x1E00FFB8   /* bgtz       $s0, .L80064758 */\n"
    ".word 0x01425020   /* add       $t2, $t2, $v0 */\n"
    ".word 0x21080200   /* addi       $t0, $t0, 0x200 */\n"
    ".word 0x01605021   /* addu       $t2, $t3, $zero */\n"
    ".word 0x2A810010   /* slti       $at, $s4, 0x10 */\n"
    ".word 0x1420FFA6   /* bnez       $at, .L80064724 */\n"
    ".word 0x22940001   /* addi      $s4, $s4, 0x1 */\n"
    ".word 0x8FB20000   /* lw         $s2, 0x0($sp) */\n"
    ".word 0x8FB30004   /* lw         $s3, 0x4($sp) */\n"
    ".word 0x8FB40008   /* lw         $s4, 0x8($sp) */\n"
    ".word 0x8FB5000C   /* lw         $s5, 0xC($sp) */\n"
    ".word 0x8FB60010   /* lw         $s6, 0x10($sp) */\n"
    ".word 0x8FB00014   /* lw         $s0, 0x14($sp) */\n"
    ".word 0x8FBF0018   /* lw         $ra, 0x18($sp) */\n"
    ".word 0x8FB1001C   /* lw         $s1, 0x1C($sp) */\n"
    ".word 0x27BD0020   /* addiu      $sp, $sp, 0x20 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_800646BC\n"
    ".set reorder\n"
);
