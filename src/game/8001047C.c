// Categories: Graphics > GTE > Projection | Graphics > Clipping
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Projects a vertex list one at a time (RTPS) and writes a clip-flag byte
 *        per vertex.
 *
 * @param (implicit) $a0 source vertices (VXY0/VZ0, 8 bytes each), $a1 output
 *        (SXY2 + SZ3, 8 bytes each), $a2 clip-flag output (1 byte per vertex),
 *        $a3 count.
 * @return In $v0: the OR of all FLAG registers seen, so the caller can test the
 *         whole batch for overflow in one go.
 *
 * @par Confirmed per-vertex step
 * lw/mtc2 $0/$1, two nops, `rtps`, then SXY2 ($14) and SZ3 ($19) are read.
 * The clip byte is assembled from three tests: SZ3 < 0x400 (bit 0x40/0x80
 * depending on the screen width 0x140 or 0xA0 selected by D_8009FA0C), the
 * x coordinate against that width and y against 0xF0. A FLAG bit 0x1C66 (the
 * GTE saturation bits, pre-shifted by 12) sets the byte to 1.
 *
 * @par Likely role
 * The single-vertex variant of the projection pass; func_800102A0 is the
 * three-at-a-time (RTPT) version with the same flag logic.
 *
 * @par Environment path
 * The batched RTPS projection step: it loads each 8-byte source vertex into
 * VXY0/VZ0, runs rtps, stores SXY2 and SZ3 into an 8-byte output slot plus
 * one clip byte per vertex, and returns the OR of every FLAG register value
 * so the caller can reject the run with one test. The clip byte is 1 when
 * FLAG has any of the 0x1C66000 error bits set; otherwise bit 0 is set when
 * SZ3 < 0x400, bit 6 when x is below the screen width (0x140 with
 * gPlayerCount == 1, otherwise 0xA0) and bit 7 when y < 0xF0. It has fifteen
 * callers across the model, effect and ribbon renderers, among them
 * func_80013F08, func_800164F4, func_8004C5C8, func_80071FA4 and
 * func_80074104.
 */
void gProjectVertexBatch(void) __asm__("func_8001047C");

__asm__(
    ".set noreorder\n"
    ".globl func_8001047C\n"
    ".ent func_8001047C\n"
    "func_8001047C:\n"
    ".word 0x240C1C66   /* addiu      $t4, $zero, 0x1C66 */\n"
    ".word 0x000C6300   /* sll        $t4, $t4, 12 */\n"
    ".word 0x3C0A800A   /* lui        $t2, %hi(D_8009FA0C) */\n"
    ".word 0x8D4AFA0C   /* lw         $t2, %lo(D_8009FA0C)($t2) */\n"
    ".word 0x240B00F0   /* addiu      $t3, $zero, 0xF0 */\n"
    ".word 0x24090001   /* addiu      $t1, $zero, 0x1 */\n"
    ".word 0x112A0002   /* beq        $t1, $t2, .L800104A0 */\n"
    ".word 0x240A0140   /* addiu     $t2, $zero, 0x140 */\n"
    ".word 0x240A00A0   /* addiu      $t2, $zero, 0xA0 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x00001021   /* addu       $v0, $zero, $zero */\n"
    ".word 0x28E10001   /* slti       $at, $a3, 0x1 */\n"
    ".word 0x14200024   /* bnez       $at, .L80010544 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A180001   /* rtps */\n"
    ".word 0x20E7FFFF   /* addi       $a3, $a3, -0x1 */\n"
    ".word 0x20C60001   /* addi       $a2, $a2, 0x1 */\n"
    ".word 0x20A50008   /* addi       $a1, $a1, 0x8 */\n"
    ".word 0x20840008   /* addi       $a0, $a0, 0x8 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x48037000   /* mfc2       $v1, $14 */\n"
    ".word 0x484FF800   /* cfc2       $t7, $31 */\n"
    ".word 0x480E9800   /* mfc2       $t6, $19 */\n"
    ".word 0x004F1025   /* or         $v0, $v0, $t7 */\n"
    ".word 0x018F7824   /* and        $t7, $t4, $t7 */\n"
    ".word 0x11E00004   /* beqz       $t7, .L8001050C */\n"
    ".word 0x240F0001   /* addiu     $t7, $zero, 0x1 */\n"
    ".word 0xA0CFFFFF   /* sb         $t7, -0x1($a2) */\n"
    ".word 0x0401FFEA   /* bgez       $zero, .L800104B0 */\n"
    ".word 0x28E10001   /* slti      $at, $a3, 0x1 */\n"
    ".word 0xACA3FFF8   /* sw         $v1, -0x8($a1) */\n"
    ".word 0xACAEFFFC   /* sw         $t6, -0x4($a1) */\n"
    ".word 0x29CF0400   /* slti       $t7, $t6, 0x400 */\n"
    ".word 0x306EFFFF   /* andi       $t6, $v1, 0xFFFF */\n"
    ".word 0x00031C02   /* srl        $v1, $v1, 16 */\n"
    ".word 0x01CA082A   /* slt        $at, $t6, $t2 */\n"
    ".word 0x00010980   /* sll        $at, $at, 6 */\n"
    ".word 0x01E17825   /* or         $t7, $t7, $at */\n"
    ".word 0x006B082A   /* slt        $at, $v1, $t3 */\n"
    ".word 0x000109C0   /* sll        $at, $at, 7 */\n"
    ".word 0x01E17825   /* or         $t7, $t7, $at */\n"
    ".word 0xA0CFFFFF   /* sb         $t7, -0x1($a2) */\n"
    ".word 0x0401FFDC   /* bgez       $zero, .L800104B0 */\n"
    ".word 0x28E10001   /* slti      $at, $a3, 0x1 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_8001047C\n"
    ".set reorder\n"
);
