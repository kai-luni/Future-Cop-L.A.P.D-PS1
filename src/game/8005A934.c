// Categories: Graphics > HUD > Radar | Utilities > Mathematics > Trigonometry
#include "game/types.h"

struct Unk5A934 {
    u8 unk00[0x50];
    s32 x;
    s32 y;
    s32 z;
    u8 unk5C[0x54];
    s16 heading;
};

extern s32 func_80095C30(s32 x, s32 z);

/**
 * @brief Computes the relative bearing from an object to a far target for
 *        the radar arrow.
 *
 * @param object Reference block; +0x50/+0x54/+0x58 the position triple,
 *        +0xB0 the s16 heading.
 * @param z, x Target coordinates.
 * @param angle Out: the bearing relative to the object's heading.
 * @return 1 when either axis distance exceeds 0x2000 (the arrow is shown),
 *         0 otherwise.
 *
 * @par Confirmed
 * dz = z - object->z, dx = x - object->x; the branchless absolute values
 * gate on 0x2000. func_80095C30(dx, dz) returns the 12-bit angle, sign-
 * extended via (v << 12) >> 12, then heading and 0x400 are subtracted.
 *
 * @par Likely role
 * The off-screen enemy indicator of the radar func_8005A9C4: only targets
 * beyond the 0x2000 dead zone get an arrow, rotated by the player heading
 * with a quarter-turn (0x400) correction.
 *
 * @par Callers
 * func_8005A9C4.
 */
s32 func_8005A934(struct Unk5A934 *object, s32 z, s32 x, s32 *angle)
{
    s32 dz;
    s32 dx;
    s32 direction;

    dz = z - object->z;
    dx = x - object->x;
    if ((((dz >> 31) + dz) ^ (dz >> 31)) > 0x2000 ||
        (((dx >> 31) + dx) ^ (dx >> 31)) > 0x2000) {
        direction = func_80095C30(dx, dz);
        direction = (direction << 12) >> 12;
        direction -= object->heading;
        direction -= 0x400;
        *angle = direction;
        return 1;
    }
    return 0;
}
