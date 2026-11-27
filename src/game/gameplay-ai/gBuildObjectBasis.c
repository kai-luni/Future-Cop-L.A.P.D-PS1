// Categories: Physics > Camera | Graphics > GTE
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk51C50View {
    s32 unk00[3];
    struct Vec3s32 up;
    struct Vec3s32 target;
};

struct Unk51C50 {
    u8 unk00[0xF0];
    s16 angle;
};

extern void gGteCrossProduct(struct Vec3s32 *up, struct Vec3s32 *target, struct Unk51C50View *view) __asm__("func_80010974");
extern void gBuildRotationMatrix(s32 arg0, s32 arg1, s32 angle, struct Unk51C50View *view) __asm__("func_800188D0");
void gBuildObjectBasis(struct Unk51C50 *object, s32 arg1, struct Vec3s32 *target, struct Unk51C50View *view) __asm__("func_80051C50");


/**
 * @brief Builds the view frame of an object: look-at form when its angle
 *        is zero, rotated form otherwise.
 *
 * @param object Object with the s16 angle at +0xF0.
 * @param arg1 Yaw passed to the rotation builder in the angled path.
 * @param target Look-at target position.
 * @param view Output view record (first triple, up, target).
 * @return Nothing.
 *
 * @par Confirmed
 * angle == 0: view->target = *target; view->up = (0, 0x10000, 0);
 * func_80010974(&up, &target, view) writes the cross product into the
 * first triple of view. Otherwise func_800188D0(arg1, 0, angle, view)
 * builds the rotated frame.
 *
 * @par Likely role
 * Camera or turret view: the zero-angle case looks at a target with a
 * world-up vector, the angled case uses a fixed yaw/roll rotation.
 *
 * @par Callers
 * func_80052410.
 *
 * @par Environment path
 * Builds an object's orientation matrix into the 3x3 block passed as the last
 * argument (the caller passes the object's own matrix at +0x2C): when the
 * object's angle field at +0xF0 is zero it copies the direction vector into
 * the third row, sets the second row to the up vector (0, 0x10000, 0) and
 * lets func_80010974 write their cross product into the first row; otherwise
 * it builds the rotation with func_800188D0 from arg1 and the +0xF0 angle.
 * Its only caller is the footprint test func_80052410, which loads the result
 * into the GTE to transform the footprint corners. It touches no globals.
 */
void gBuildObjectBasis(struct Unk51C50 *object, s32 arg1, struct Vec3s32 *target,
                   struct Unk51C50View *view)
{
    if (object->angle == 0) {
        view->target = *target;
        view->up.x = 0;
        view->up.y = 0x10000;
        view->up.z = 0;
        gGteCrossProduct(&view->up, &view->target, view);
    } else {
        gBuildRotationMatrix(arg1, 0, object->angle, view);
    }
}
