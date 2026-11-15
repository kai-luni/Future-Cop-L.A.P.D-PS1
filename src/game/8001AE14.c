// Categories: Utilities > Mathematics > Geometry | Graphics > GTE > Matrix Math
#include "game/types.h"

/**
 * @brief Software 3x3 matrix-vector transform with the shared fixed-point
 *        scaling.
 *
 * @param matrix Nine s32 words, row-major.
 * @param vector Three s32 components.
 * @param out Three s32 results.
 * @return Nothing.
 *
 * @par Confirmed
 * Both the vector and every matrix word are pre-scaled >> 2; each output
 * row is the dot product >> 12. No GTE involved, no saturation.
 *
 * @par Likely role
 * The CPU-side rotation of the culling path func_80013F08: the same >>2/>>12
 * convention as the GTE matrix helpers keeps the software and hardware
 * paths in the same fixed-point range.
 *
 * @par Callers
 * func_80013F08.
 */
void func_8001AE14(s32 *matrix, s32 *vector, s32 *out)
{
    s32 x;
    s32 y;
    s32 z;

    x = vector[0] >> 2;
    y = vector[1] >> 2;
    z = vector[2] >> 2;
    out[0] = (x * (matrix[0] >> 2) + y * (matrix[1] >> 2) + z * (matrix[2] >> 2)) >> 12;
    out[1] = (x * (matrix[3] >> 2) + y * (matrix[4] >> 2) + z * (matrix[5] >> 2)) >> 12;
    out[2] = (x * (matrix[6] >> 2) + y * (matrix[7] >> 2) + z * (matrix[8] >> 2)) >> 12;
}
