// Categories: Utilities > Mathematics | Graphics > Geometry
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
void gMulMatrixVector(volatile s32 *matrix, volatile s32 *input, s32 *output) __asm__("func_8001A81C");


/**
 * @brief Software 3x3 matrix-times-vector product in 16.16 fixed point.
 *
 * @param matrix Nine s32 coefficients, column-major (columns at 0/3/6).
 * @param input Three s32 components.
 * @param output Receives the three result components.
 * @return Nothing.
 *
 * @par Confirmed
 * output[0] = x*m[0] + y*m[3] + z*m[6], output[1] = x*m[1] + y*m[4] +
 * z*m[7], output[2] = x*m[2] + y*m[5] + z*m[8]. Each product uses the FMUL
 * inline-asm macro: mult, then (hi << 16) | (lo >> 16) - the middle 32
 * bits of the 64-bit product, i.e. product >> 16 (16 fractional bits).
 * The matrix and input pointers are volatile-qualified.
 *
 * @par Likely role
 * CPU-side companion of the GTE matrix ops, used where the geometry path
 * needs a transform without the coprocessor.
 *
 * @par Callers
 * func_80013C80, func_800177AC, func_80018860, func_80074104, func_80075A84.
 *
 * @par Environment path
 * Software matrix-times-vector in 16.16 used whenever the GTE must not be
 * disturbed, e.g. by the light direction update and the polygon pass.
 */
void gMulMatrixVector(volatile s32 *matrix, volatile s32 *input, s32 *output)
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
