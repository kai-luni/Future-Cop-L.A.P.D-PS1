// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

/**
 * @brief Handwritten squared-length helper: sum of the three squared
 *        components, each product's middle 32 bits (>> 16 of the 64-bit
 *        square).
 *
 * @param arg0 Vector of three s32 components.
 * @return x*x + y*y + z*z in 16.16-derived scale.
 *
 * @par Confirmed
 * Per component mult + (mfhi << 16 | mflo >> 16), the three results
 * summed; the existing source comment documents the same formula. No
 * square root.
 *
 * @par Likely role
 * The squared-distance primitive of the tail/whip physics (func_8005C344,
 * func_8005B5A0) and the player tick: comparing squared lengths avoids
 * the root, the reciprocal helpers consume the same scale.
 *
 * @par Callers
 * func_80036624, func_800549F0, func_8005B5A0, func_8005C344, func_8005F018.
 */
/* Handwritten retail routine: returns sum(((s64)v[i] * v[i]) >> 16) for i = 0..2. */
s32 func_80019FCC(s32 *arg0)
{
    __asm__ volatile(
        "lw $13, 0x0($4)\n\t"
        "lw $14, 0x4($4)\n\t"
        "lw $15, 0x8($4)\n\t"
        "mult $13, $13\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $2, $10, $11\n\t"
        "mult $14, $14\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $12, $10, $11\n\t"
        "mult $15, $15\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $10, $9, 16\n\t"
        "srl $11, $8, 16\n\t"
        "or $13, $10, $11\n\t"
        "add $2, $2, $12\n\t"
        "add $2, $2, $13"
        :
        :
        : "$2", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "memory");
}