// Categories: Gameplay > Object Spawning | Gameplay > Entity Update
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk4B180Shape {
    u8 unk00[0x0C];
    struct Vec3s32 position;
    s16 radius;
};

struct Unk4B180Hit {
    s32 unk00;
    struct Vec3s32 position;
    u8 unk10[0x30];
    s16 strength;
    u8 unk42[7];
    u8 mode;
    u8 unk4A[6];
};

struct Unk4B180 {
    u8 unk00[0x0F];
    u8 flags0F;
    u8 flags10;
    u8 unk11[0x0B];
    void *owner;
    u8 unk20[0x44];
    u8 kind;
    u8 unk65[0x1B];
    struct Unk4B180Shape *shape;
};

extern s32 func_8001A314(s32 *vector);
extern s32 func_8003E984(void *owner, s32 arg1, u8 kind, struct Unk4B180Hit *hit, s32 arg4);

/**
 * @brief Marks a triggered node and spawns an object from its shape's position vector.
 *
 * Returns unless object->shape->radius < 0; sets flag 1 in flags0F, normalizes the shape's
 * position vector via func_8001A314 to derive strength and direction, and calls func_8003E984.
 *
 * @param object Node holding flags0F (+0x0F), flags10 (+0x10), owner (+0x1C), kind (+0x64), and shape (+0x80).
 *
 * @par Globals
 * None directly.
 * @par Control flow
 * Early returns if shape->radius >= 0; sets flags0F |= 1, returns if kind == 0, else normalizes vector, forces y >= 0, builds local hit struct, and spawns.
 * @par Calls
 * func_8001A314 - normalizes hit.position in place and returns its length (shifted >> 12 into hit.strength); func_8003E984 - spawns object->kind.
 * @par Side effects
 * Sets bit 0 of object->flags0F and spawns a child/effect entity via func_8003E984.
 * @par Callers
 * Confirmed: func_8004C2B8 (called on linked list nodes with node->flags & 0x08).
 */
void func_8004B180(struct Unk4B180 *object)
{
    struct Unk4B180Hit hit;
    struct Unk4B180Shape *shape;
    s32 length;

    if (object->shape->radius >= 0) {
        return;
    }
    object->flags0F |= 1;
    if (object->kind == 0) {
        return;
    }
    shape = object->shape;
    hit.position = shape->position;
    length = func_8001A314(&hit.position.x);
    if (hit.position.y < 0) {
        hit.position.y = -hit.position.y;
    }
    hit.strength = length >> 12;
    hit.mode = 2;
    func_8003E984(object->owner, 0, object->kind, &hit, (object->flags10 >> 1) & 1);
}
