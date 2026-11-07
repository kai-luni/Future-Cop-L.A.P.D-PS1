#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

void func_8001A81C(volatile s32 *matrix, volatile s32 *input, s32 *output)
{
    register s32 x __asm__("$12");
    register s32 y __asm__("$11");
    register s32 z __asm__("$10");
    register s32 b __asm__("$2");
    register u32 p0 __asm__("$3");
    register u32 p1 __asm__("$7");
    register u32 p2 __asm__("$5");

    x = input[0];
    b = matrix[0];
    FMUL(p0, x, b);
    y = input[1];
    b = matrix[3];
    FMUL(p1, y, b);
    z = input[2];
    b = matrix[6];
    FMUL(b, z, b);
    output[0] = p0 + p1 + b;

    b = matrix[1];
    FMUL(p0, x, b);
    b = matrix[4];
    FMUL(p2, y, b);
    b = matrix[7];
    FMUL(b, z, b);
    output[1] = p0 + p2 + b;

    b = matrix[2];
    FMUL(p0, x, b);
    b = matrix[5];
    FMUL(p2, y, b);
    b = matrix[8];
    FMUL(b, z, b);
    output[2] = p0 + p2 + b;
}
