#include "game/types.h"

extern s32 D_8009C810;

__asm__(
    ".text\n\t"
    ".set\tnoreorder\n\t"
    ".set\tnoat\n\t"
    ".globl\tfunc_80091240\n\t"
    ".ent\tfunc_80091240\n"
    "func_80091240:\n\t"
    "lui\t$2, %hi(D_8009C810)\n\t"
    "lw\t$2, %lo(D_8009C810)($2)\n\t"
    "lui\t$1, %hi(D_8009C810)\n\t"
    "jr\t$31\n\t"
    "sw\t$4, %lo(D_8009C810)($1)\n\t"
    ".set\tat\n\t"
    ".set\treorder\n\t"
    ".end\tfunc_80091240\n"
);
