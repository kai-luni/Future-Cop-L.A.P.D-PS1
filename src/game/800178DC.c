// Categories: Graphics > Camera > Relative Transform | Graphics > GTE
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Camera {
    u8 unk00[0x28];
    u8 matrix[0x54];
    struct Vec3s32 eye;
};

struct Unk178DC {
    u8 unk00[0x24];
    struct Vec3s32 translation;
};

extern struct Camera *gCamera __asm__("D_8009F9E4");

extern void func_80010820(u8 *matrix);
extern void func_8001006C(struct Unk178DC *object, s32 arg1, s32 count);

/**
 * @brief Transforms four vertices of an object relative to the camera eye.
 *
 * @param object Object: its translation at +0x24 (three 32-bit words).
 * @param arg1 Forwarded to func_8001006C as the second argument.
 * @return Nothing; the object's translation is restored before returning.
 *
 * @par Confirmed
 * saved = object->translation; then the camera eye (D_8009F9E4->eye) is
 * subtracted from it; func_80010820(camera->matrix) loads the camera matrix into
 * the GTE and func_8001006C(object, arg1, 4) transforms exactly four vertices;
 * afterwards the saved translation is written back.
 *
 * @par Likely role
 * The "place this object in camera space without touching its stored position"
 * step, used where a model has to be drawn relative to the viewer.
 */
void func_800178DC(struct Unk178DC *object, s32 arg1)
{
    struct Vec3s32 saved;
    struct Camera *camera;

    saved = object->translation;
    camera = gCamera;
    object->translation.x -= camera->eye.x;
    object->translation.y -= camera->eye.y;
    object->translation.z -= camera->eye.z;
    func_80010820(camera->matrix);
    func_8001006C(object, arg1, 4);
    object->translation = saved;
}
