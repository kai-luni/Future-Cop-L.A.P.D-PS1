// Categories: Graphics > GTE > Vector Product | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Cross product with a temporary matrix, restoring the caller's matrix
 *        afterwards.
 *
 * @param (implicit) $a0 matrix words, $a1 vector words, $a2 destination.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * cfc2 saves the rotation matrix words $0/$2/$4, the new matrix and the vector are
 * loaded with sra 4 scaling, `op 1` (0Ch, sf = 1) computes the cross product,
 * MAC1..MAC3 are stored shifted left by 4, and the three saved matrix words are
 * written back with ctc2.
 *
 * @par Compare
 * func_80010974 performs the same computation without the save/restore, so it is
 * the variant for callers that own the matrix.
 *
 * @par Callers
 * None in the symbol table (inlined).
 *
 * @par Environment path
 * Cross product variant that restores the caller's GTE matrix afterwards. It
 * has no caller in the US binary, so only the documented contract matters for
 * the port.
 */
void gGteCrossProductPreserveRotation(void) __asm__("func_800108D8");

__asm__(
    ".set noreorder\n"
    ".globl func_800108D8\n"
    ".ent func_800108D8\n"
    "func_800108D8:\n"
    ".word 0x484D0000   /* cfc2       $t5, $0 */\n"
    ".word 0x484E1000   /* cfc2       $t6, $2 */\n"
    ".word 0x484F2000   /* cfc2       $t7, $4 */\n"
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
    ".word 0x48CD0000   /* ctc2       $t5, $0 */\n"
    ".word 0x48CE1000   /* ctc2       $t6, $2 */\n"
    ".word 0x48CF2000   /* ctc2       $t7, $4 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_800108D8\n"
    ".set reorder\n"
);
