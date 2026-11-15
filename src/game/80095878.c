// Categories: Graphics > GTE > Initialisation | Platform > GTE
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Initialises the GTE for the standard 320x240 projection.
 *
 * @return Nothing.
 *
 * @par Confirmed sequence
 * 1. $ra is parked in D_8009DF10 around the call to func_80095DB0 (a routine that
 *    runs with the caller's $ra stashed, i.e. it clobbers the return address).
 * 2. COP0 status ($12) gets bit 30 set (0x40000000) - this enables COP2, the GTE.
 * 3. Control registers: ZSF3 = 0x155, ZSF4 = 0x100, H = 0x3E8, DQA = -0x1062,
 *    DQB = 0x1400000, OFX = OFY = 0.
 *
 * @par Why these numbers
 * H = 1000, DQA/DQB are the standard linear-interpolation coefficients for a
 * 320x240 screen (the usual SDK values), and ZSF3/ZSF4 are the average-Z scale
 * factors derived from the screen area.
 *
 * @par Callers
 * func_8001E4E4, once during boot.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095878\n"
    ".ent func_80095878\n"
    "func_80095878:\n"
    ".word 0x3C01800A   /* lui        $at, %hi(D_8009DF10) */\n"
    ".word 0xAC3FDF10   /* sw         $ra, %lo(D_8009DF10)($at) */\n"
    ".word 0x0C02576C   /* jal        func_80095DB0 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x3C1F800A   /* lui        $ra, %hi(D_8009DF10) */\n"
    ".word 0x8FFFDF10   /* lw         $ra, %lo(D_8009DF10)($ra) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x40026000   /* mfc0       $v0, $12 */\n"
    ".word 0x3C034000   /* lui        $v1, (0x40000000 >> 16) */\n"
    ".word 0x00431025   /* or         $v0, $v0, $v1 */\n"
    ".word 0x40826000   /* mtc0       $v0, $12 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x24080155   /* addiu      $t0, $zero, 0x155 */\n"
    ".word 0x48C8E800   /* ctc2       $t0, $29 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x24080100   /* addiu      $t0, $zero, 0x100 */\n"
    ".word 0x48C8F000   /* ctc2       $t0, $30 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x240803E8   /* addiu      $t0, $zero, 0x3E8 */\n"
    ".word 0x48C8D000   /* ctc2       $t0, $26 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x2408EF9E   /* addiu      $t0, $zero, -0x1062 */\n"
    ".word 0x48C8D800   /* ctc2       $t0, $27 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x3C080140   /* lui        $t0, (0x1400000 >> 16) */\n"
    ".word 0x48C8E000   /* ctc2       $t0, $28 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x48C0C000   /* ctc2       $zero, $24 */\n"
    ".word 0x48C0C800   /* ctc2       $zero, $25 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80095878\n"
    ".set reorder\n"
);
