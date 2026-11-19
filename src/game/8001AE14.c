// Categories: Utilities > Mathematics > Geometry | Graphics > GTE > Matrix Math
#include "game/types.h"

void gMultiplyMatrixVectorWithPreshift(s32 *matrix, s32 *vector, s32 *out) __asm__("func_8001AE14");

/**
 * @brief Applies a 3-by-3 fixed-point matrix to a vector with a preshift on
 * both inputs.
 *
 * @param matrix Nine matrix words grouped in three rows.
 * @param vector Three signed vector components.
 * @param out Destination for three result words.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Arithmetic-shifts all loaded components and coefficients by two before
 * multiplication. For each contiguous matrix row sums three low product words,
 * shifts the sum right by 12, and stores the result.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gProjectModelInstance.
 *
 * @par Role and path
 * The model path at gProjectModelInstance uses this software matrix-vector
 * multiply.
 *
 * @par Unknown
 * The caller controls the input orientation and the coordinate frame of the
 * result.
 */
void gMultiplyMatrixVectorWithPreshift(s32 *matrix, s32 *vector, s32 *out)
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
