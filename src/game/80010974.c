// Categories: Graphics > GTE > Vector Product | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Cross product of two vectors with a caller-supplied matrix, 32-bit out.
 *
 * @param (implicit) $a0 matrix pointer, $a1 vector pointer, $a2 destination.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The three words of *a0 are shifted right by 4 and written to the rotation
 * matrix ($0, $2, $4); the three words of *a1 are shifted right by 4 into
 * IR1..IR3 (mtc2 $9/$10/$11); `op 1` (real opcode 0Ch, sf = 1) computes the cross
 * product; MAC1..MAC3 are read back, shifted left by 4 and stored as three
 * words.
 *
 * @par Compare
 * func_800108D8 does the same but saves and restores the caller's rotation
 * matrix; func_80095B64 is the IR-vector form.
 *
 * @par Callers
 * func_800192AC, func_80019310, func_8001937C, func_800193F8, gGuidedProjectileUpdate,
 * func_80051C50, func_80053A74.
 *
 * @par Environment path
 * Cross product used to build a normal from two vectors: the light setups
 * (func_800192AC, func_80019310, func_8001937C, func_800193F8) and the
 * map-cell drawers (func_80035B7C, func_80051C50, func_80053A74).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80010974\n"
    ".ent func_80010974\n"
    "func_80010974:\n"
    ".word 0x8C890000   /* lw         $t1, 0x0($a0) */\n"
    ".word 0x8C8A0004   /* lw         $t2, 0x4($a0) */\n"
    ".word 0x8C8B0008   /* lw         $t3, 0x8($a0) */\n"
    ".word 0x00094903   /* sra        $t1, $t1, 4 */\n"
    ".word 0x000A5103   /* sra        $t2, $t2, 4 */\n"
    ".word 0x000B5903   /* sra        $t3, $t3, 4 */\n"
    ".word 0x48C90000   /* ctc2       $t1, $0 */\n"
    ".word 0x48CA1000   /* ctc2       $t2, $2 */\n"
    ".word 0x48CB2000   /* ctc2       $t3, $4 */\n"
    ".word 0x8CA90000   /* lw         $t1, 0x0($a1) */\n"
    ".word 0x8CAA0004   /* lw         $t2, 0x4($a1) */\n"
    ".word 0x8CAB0008   /* lw         $t3, 0x8($a1) */\n"
    ".word 0x00094903   /* sra        $t1, $t1, 4 */\n"
    ".word 0x000A5103   /* sra        $t2, $t2, 4 */\n"
    ".word 0x000B5903   /* sra        $t3, $t3, 4 */\n"
    ".word 0x488B5800   /* mtc2       $t3, $11 */\n"
    ".word 0x48894800   /* mtc2       $t1, $9 */\n"
    ".word 0x488A5000   /* mtc2       $t2, $10 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4B78000C   /* op         1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4809C800   /* mfc2       $t1, $25 */\n"
    ".word 0x480AD000   /* mfc2       $t2, $26 */\n"
    ".word 0x480BD800   /* mfc2       $t3, $27 */\n"
    ".word 0x00094900   /* sll        $t1, $t1, 4 */\n"
    ".word 0x000A5100   /* sll        $t2, $t2, 4 */\n"
    ".word 0x000B5900   /* sll        $t3, $t3, 4 */\n"
    ".word 0xACC90000   /* sw         $t1, 0x0($a2) */\n"
    ".word 0xACCA0004   /* sw         $t2, 0x4($a2) */\n"
    ".word 0xACCB0008   /* sw         $t3, 0x8($a2) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80010974\n"
    ".set reorder\n"
);
