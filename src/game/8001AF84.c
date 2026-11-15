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

/**
 * @brief Software-transforms a vertex array by a 3x4 matrix (the CPU twin
 *        of the GTE RTPT path).
 *
 * @param matrix 12 words: 9 rotation + 3 translation, all >> 4 on load.
 * @param source Input vertices (s16 x/y/z/pad).
 * @param destination Output vertices, same layout.
 * @param count Vertex count (<= 0 returns).
 * @return Nothing.
 *
 * @par Confirmed
 * Per vertex: out.x = (x*m0 + y*m3 + z*m6) >> 12 + tx, and the m1/m4/m7
 * and m2/m5/m8 rows for y/z. The >> 4 matrix preload and >> 12 product
 * shift keep the 16.16 convention.
 *
 * @par Likely role
 * The CPU fallback transform of the light node builder func_80049980:
 * small vertex sets (the 5-point diamond) transform cheaper here than via
 * a GTE round trip.
 *
 * @par Callers
 * func_80049980.
 */
void func_8001AF84(struct TransformMatrix *matrix, struct TransformPoint *source,
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
