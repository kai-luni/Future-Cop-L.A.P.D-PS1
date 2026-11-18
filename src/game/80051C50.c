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
 * Builds the view frame of an object into a three-vector record: when the
 * angle at +0xF0 is zero it stores the target triple, sets the up vector to
 * (0, 0x10000, 0) and calls func_80010974 to write the cross product into the
 * first triple, otherwise it builds a rotated frame with func_800188D0
 * passing arg1 as yaw and the object angle as roll. Its only caller is
 * func_80052410, and the file describes the result as a camera or turret
 * view. It touches no globals and only writes through the view pointer.
 */
void func_80051C50(struct Unk51C50 *object, s32 arg1, struct Vec3s32 *target,
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
