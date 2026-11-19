// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

/* Handwritten retail routine: returns sum(((s64)v[i] * v[i]) >> 16) for i = 0..2. */

s32 gSquaredLength3Q16(s32 *arg0) __asm__("func_80019FCC");

/**
 * @brief Computes the squared length of a three-component Q16 vector.
 *
 * @param arg0 Three signed 16.16 components.
 * @return The sum of three individually shifted squares.
 *
 * @par Confirmed mechanics
 * Performs three signed MULT instructions. Combines HI << 16 with LO >> 16 for
 * each square, then sums the three terms with signed ADD.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80036624, func_800549F0,
 * func_8005B5A0, func_8005C344, func_8005F018.
 *
 * @par Role and path
 * Gameplay distance tests use this helper to compare squared magnitudes.
 *
 * @par Unknown
 * The caller supplies inputs whose scaled squares and sum fit the output word.
 */
s32 gSquaredLength3Q16(s32 *arg0)
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