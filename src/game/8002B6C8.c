#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk8002B6C8 {
    u8 unk00[0x5C];
    s32 x;
    s32 y;
    s32 z;
    u8 unk68[0xF4];
    s16 unk15C;
    u8 unk15E;
    u8 unk15F[5];
    u8 unk164;
};

extern s32 func_80095C30(s32 x, s32 z);
extern void func_8002B588(struct Unk8002B6C8 *object, s32 angle, s32 arg2);
extern void func_8002B0EC(struct Unk8002B6C8 *object, s32 arg1);

void func_8002B6C8(struct Unk8002B6C8 *object, struct Unk8002B6C8 *other, s32 arg2)
{
    struct Vec3s32 delta;
    s32 angle;
    s32 diff;

    delta.x = other->x - object->x;
    delta.y = other->y - object->y;
    delta.z = other->z - object->z;
    angle = func_80095C30(delta.x, delta.z) & 0xFFF;
    diff = angle - object->unk15C;
    if ((((diff >> 31) + diff) ^ (diff >> 31)) > 0x40) {
        func_8002B588(object, angle, arg2);
    } else if (object->unk15E != object->unk164) {
        func_8002B0EC(object, object->unk164);
    }
}
