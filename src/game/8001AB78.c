#include "game/types.h"

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
