// Categories: Graphics > GTE > Software Transform | Utilities > Mathematics
#include "game/types.h"

struct TransformMatrix {
    s32 values[12];
};

struct TransformPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

void gTransformS16PointArray(struct TransformMatrix *matrix, struct TransformPoint *source,
                   struct TransformPoint *destination, s32 count) __asm__("func_8001AF84");

/**
 * @brief Transforms an array of packed halfword points using a fixed-point
 * affine matrix.
 *
 * @param matrix Nine coefficient words followed by three translation words.
 * @param source Input point array with XYZ at offsets +0, +2, and +4 of each
 *     eight-byte record.
 * @param destination Output point array with the same stride.
 * @param count Number of points.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Returns immediately for nonpositive count. Shifts all 12 matrix words right
 * by four. For each eight-byte point, multiplies three signed halfwords by
 * each coefficient column, shifts each sum right by 12, adds the corresponding
 * shifted translation, and stores three halfwords.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80049980.
 *
 * @par Role and path
 * The geometry helper at func_80049980 uses this software point transform.
 *
 * @par Unknown
 * The fourth halfword in each eight-byte record is left intact and its
 * interpretation is caller-specific.
 */
void gTransformS16PointArray(struct TransformMatrix *matrix, struct TransformPoint *source,
                   struct TransformPoint *destination, s32 count)
{
    s32 m0;
    s32 m1;
    s32 m2;
    s32 m3;
    s32 m4;
    s32 m5;
    s32 m6;
    s32 m7;
    s32 m8;
    s32 tx;
    s32 ty;
    s32 tz;

    if (count <= 0) {
        return;
    }

    m0 = matrix->values[0] >> 4;
    m1 = matrix->values[1] >> 4;
    m2 = matrix->values[2] >> 4;
    m3 = matrix->values[3] >> 4;
    m4 = matrix->values[4] >> 4;
    m5 = matrix->values[5] >> 4;
    m6 = matrix->values[6] >> 4;
    m7 = matrix->values[7] >> 4;
    m8 = matrix->values[8] >> 4;
    tx = matrix->values[9] >> 4;
    ty = matrix->values[10] >> 4;
    tz = matrix->values[11] >> 4;

    do {
        s32 x;
        s32 y;
        s32 z;

        x = source->x;
        y = source->y;
        z = source->z;
        destination->x = ((x * m0 + y * m3 + z * m6) >> 12) + tx;
        destination->y = ((x * m1 + y * m4 + z * m7) >> 12) + ty;
        destination->z = ((x * m2 + y * m5 + z * m8) >> 12) + tz;
        source++;
        destination++;
        count--;
    } while (count != 0);
}
