#include "game/types.h"

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