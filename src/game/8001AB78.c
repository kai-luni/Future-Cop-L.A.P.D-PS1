// Categories: Utilities > Mathematics > Vector | Platform > Handwritten ASM
#include "game/types.h"

/**
 * @brief Handwritten in-place vector scale: multiplies three s32 components
 *        by a 16.16 fixed-point scalar.
 *
 * @param arg0 ($4) The scale factor (16.16).
 * @param arg1 ($5) Pointer to the three-component vector, updated in place.
 * @return Nothing.
 *
 * @par Confirmed
 * Per component: mult with the scale, result = (mflo >> 16) | (mfhi << 16)
 * (the middle 32 bits of the 64-bit product), stored back before the next
 * component is loaded. The final store sits in the jr delay slot.
 *
 * @par Likely role
 * The hot-path vector scale of the mover integrators and the camera rig:
 * handwritten because the compiler would spill the wide product; the same
 * bit-extraction pattern appears in the C FMUL macros.
 *
 * @par Callers
 * func_800324E0, func_80032F3C, func_8003A160, func_8004F764, func_80050E0C,
 * func_800513D0, func_80051580, func_800569EC, func_8005EE04.
 *
 * @par Environment path
 * Hand-written in-place scale of a three-component vector by a 16.16 factor
 * (argument order: scale, vector). gUpdateChaseCamera (func_800569EC,
 * 800569EC.c:505) uses it to shorten the eye-offset delta to the per-frame
 * step elapsed*5<<4 when the remaining distance is longer, which is the
 * camera's offset smoothing. Other callers are gUpdateHoverEnemy,
 * gUpdateForceField, func_80032F3C, func_8004F764, func_80050E0C,
 * func_800513D0, func_80051580 and func_8005EE04.
 */
void gVectorScaleInPlace(void) __asm__("func_8001AB78");

__asm__(
    ".text\n\t"
    ".align 2\n\t"
    ".globl func_8001AB78\n\t"
    ".ent func_8001AB78\n"
    "func_8001AB78:\n\t"
    ".set noreorder\n\t"
    "lw $2,0($5)\n\tnop\n\tmult $2,$4\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor $3,$9,$8\n\t"
    "lw $2,4($5)\n\tsw $3,0($5)\n\tmult $2,$4\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor $3,$9,$8\n\t"
    "lw $2,8($5)\n\tsw $3,4($5)\n\tmult $2,$4\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor $3,$9,$8\n\t"
    "jr $31\n\tsw $3,8($5)\n\t.set reorder\n\t"
    ".end func_8001AB78\n");
