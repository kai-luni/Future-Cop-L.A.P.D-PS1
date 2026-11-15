// Categories: Graphics > Lighting > Light Source | Graphics > Scene Node
#include "game/types.h"

struct Unk752E0Light {
    u8 unk00[9];
    u8 red;
    u8 green;
    u8 blue;
};

struct Unk753D0 {
    s32 unk00;
    s32 x;
    s32 y;
    s32 z;
    s16 size;
    u8 unk12[3];
    u8 red;
    u8 green;
    u8 blue;
    u8 unk18[2];
    s16 angle;
    u8 unk1C[8];
    struct Unk752E0Light *light;
};

extern void func_80049980(struct Unk752E0Light *light, s32 x, s32 z, s32 y, s32 width,
                          s32 depth, s32 angle, s32 arg7, s32 arg8);
extern void func_800752E0(struct Unk753D0 *object);

/**
 * @brief Updates (or creates) the light source of a scene object.
 *
 * @param object Node: position at +4/+8/+0xC, size at +0x10, colour at +0x15..+0x17,
 *               angle at +0x1A and an optional light pointer at +0x24.
 * @return Nothing.
 *
 * @par Confirmed flow
 * size = object->size << 5. When a light exists, its colour bytes are refreshed
 * from the object and func_80049980(light, x, z, y, size, size, angle, 0, 0)
 * re-positions it - note the deliberate x/z/y swap, the light lives in the other
 * axis convention. When there is no light and the radius is larger than 0x500,
 * func_800752E0(object) creates one.
 *
 * @par Likely role
 * The per-frame light update of the dynamic lights: only objects above a radius
 * threshold get a real light, everything smaller is lit statically - a
 * performance decision, not a visual one.
 *
 * @par Callers
 * func_80075A84 (the polygon renderer).
 */
void func_800753D0(struct Unk753D0 *object)
{
    s32 size;

    size = object->size << 5;
    if (object->light != 0) {
        object->light->red = object->red;
        object->light->green = object->green;
        object->light->blue = object->blue;
        func_80049980(object->light, object->x, object->z, object->y, size, size,
                      object->angle, 0, 0);
    } else if (size > 0x500) {
        func_800752E0(object);
    }
}
