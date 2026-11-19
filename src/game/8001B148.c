// Categories: Graphics > Geometry > Matrix Transform | Utilities > Mathematics
#include "game/types.h"

struct RotationMatrix {
    s32 values[9];
};

struct RotationPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

void gRotateS16PointArray(struct RotationMatrix *matrix, struct RotationPoint *source,
                   struct RotationPoint *destination, s32 count) __asm__("func_8001B148");

/**
 * @brief Transforms an array of packed halfword points using a fixed-point
 * 3-by-3 matrix.
 *
 * @param matrix Nine 32-bit coefficient words.
 * @param source Input point array with XYZ halfwords in eight-byte records.
 * @param destination Output point array with the same layout.
 * @param count Number of points.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Returns immediately for nonpositive count. Shifts nine matrix words right by
 * four. For each eight-byte source record, loads three signed halfwords,
 * shifts each three-product column sum right by 12, and stores three
 * halfwords.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gHandleLightObjectMsg,
 * func_80042AF4, gSpawnMeshFractureDebris, gHandleActor59Msg,
 * gInitEmitterParticle.
 *
 * @par Role and path
 * Gameplay and geometry callers use this translation-free software rotation
 * path.
 *
 * @par Unknown
 * The fourth halfword in each record is left intact and its interpretation is
 * caller-specific.
 */
void gRotateS16PointArray(struct RotationMatrix *matrix, struct RotationPoint *source,
                   struct RotationPoint *destination, s32 count)
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

    do {
        s32 x;
        s32 y;
        s32 z;

        x = source->x;
        y = source->y;
        z = source->z;
        destination->x = (x * m0 + y * m3 + z * m6) >> 12;
        destination->y = (x * m1 + y * m4 + z * m7) >> 12;
        destination->z = (x * m2 + y * m5 + z * m8) >> 12;
        source++;
        destination++;
        count--;
    } while (count != 0);
}
