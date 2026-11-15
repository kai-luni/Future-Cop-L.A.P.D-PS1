// Categories: Graphics > GTE > Vector Batch Transform | Graphics > Geometry
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Batch vertex transform with 32-bit results and an output stride of 0xC.
 *
 * @param (implicit) $a0 source array (8 bytes per vertex), $a1 destination
 *        (12 bytes per vertex), $a2 count.
 * @return Nothing.
 *
 * @par Confirmed
 * `mvmva 1,0,0,0,0` (v = 0, i.e. the V0 register pair), then the results are
 * read from MAC1..MAC3 ($25/$26/$27), each shifted left by 4 (a 4-bit fixed
 * point scale) and stored as three *words*; Y is negated before the shift.
 * Destination advances by 0xC per vertex.
 *
 * @par Likely role
 * The high-precision variant of func_8001010C: full 32-bit coordinates for
 * geometry that is transformed again later (it is the most used of the family,
 * 39 call sites, all over the polygon and effect code).
 *
 * @par Callers
 * 39 sites, including func_800164F4, func_80065834 and the whole 0x8002-0x8005
 * range of object renderers.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80010234\n"
    ".ent func_80010234\n"
    "func_80010234:\n"
    ".word 0x18C00018   /* blez       $a2, .L80010298 */\n"
    ".word 0x8C880000   /* lw        $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x48880000   /* mtc2       $t0, $0 */\n"
    ".word 0x48890800   /* mtc2       $t1, $1 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4A480012   /* mvmva      1, 0, 0, 0, 0 */\n"
    ".word 0x20C6FFFF   /* addi       $a2, $a2, -0x1 */\n"
    ".word 0x20A5000C   /* addi       $a1, $a1, 0xC */\n"
    ".word 0x20840008   /* addi       $a0, $a0, 0x8 */\n"
    ".word 0x8C880000   /* lw         $t0, 0x0($a0) */\n"
    ".word 0x8C890004   /* lw         $t1, 0x4($a0) */\n"
    ".word 0x480BC800   /* mfc2       $t3, $25 */\n"
    ".word 0x480CD000   /* mfc2       $t4, $26 */\n"
    ".word 0x480DD800   /* mfc2       $t5, $27 */\n"
    ".word 0x000B5900   /* sll        $t3, $t3, 4 */\n"
    ".word 0xACABFFF4   /* sw         $t3, -0xC($a1) */\n"
    ".word 0x000C6022   /* neg        $t4, $t4 */\n"
    ".word 0x000C6100   /* sll        $t4, $t4, 4 */\n"
    ".word 0xACACFFF8   /* sw         $t4, -0x8($a1) */\n"
    ".word 0x000D6900   /* sll        $t5, $t5, 4 */\n"
    ".word 0x28C10001   /* slti       $at, $a2, 0x1 */\n"
    ".word 0x1020FFEB   /* beqz       $at, .L80010240 */\n"
    ".word 0xACADFFFC   /* sw        $t5, -0x4($a1) */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80010234\n"
    ".set reorder\n"
);
