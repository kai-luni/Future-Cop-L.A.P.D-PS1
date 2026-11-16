// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

/**
 * @brief Computes the 16.16 fixed-point squared length of a 2-element 32-bit vector.
 *
 * Loads arg0[0] and arg0[1], squares each via signed 32x32->64 mult, extracts ((hi << 16) |
 * (lo >> 16)) for each term (shifting right by 16), and returns the sum of the two squares.
 *
 * @param arg0 Pointer to a two-element s32 vector (arg0[0], arg0[1]) in 16.16 fixed point.
 * @return Sum of squares ((arg0[0] * arg0[0]) >> 16) + ((arg0[1] * arg0[1]) >> 16) in $v0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line inline MIPS assembly multiplication and shift-combine sequence; no branches.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8001A7AC (0x8001A7BC, passes squared length into func_80095900 for 2D vector normalization) / Likely: none / Unknown: none.
 */
s32 func_8001A030(s32 *arg0)
{
    __asm__ volatile(
        "lw $13, 0x0($4)\n\t"
        "lw $14, 0x4($4)\n\t"
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
        "add $2, $2, $12"
        : : : "$2", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "memory");
}
