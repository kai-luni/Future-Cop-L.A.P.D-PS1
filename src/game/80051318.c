#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk80019A20 {
    u8 unk00[8];
    s32 unk08;
};

struct Unk51318Arg1 {
    s32 unk00;
    u8 unk04[0x1C];
    s32 unk20;
};

struct Unk51318 {
    struct Unk51318Arg1 *unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    struct Unk80019A20 *unk1C;
    struct Unk80019A20 *unk20;
    s32 unk24;
    s32 unk28;
    struct Vec3 unk2C;
};

extern struct Unk80019A20 *func_80019A20(u32 tag, s32 arg1, s32 arg2);

void func_80051318(struct Unk51318 *obj, struct Unk51318Arg1 *arg1, struct Vec3 *arg2)
{
    struct Unk80019A20 *res;
    struct Unk80019A20 *prev;
    s32 value;
    s32 x;
    s32 y;
    s32 z;

    obj->unk00 = arg1;
    if (arg1 != 0) {
        obj->unk1C = func_80019A20(0x61415048, arg1->unk00, arg1->unk20);
        res = func_80019A20(0x74415048, obj->unk00->unk00, obj->unk00->unk20);
        value = res->unk08;
        prev = obj->unk1C;
        obj->unk24 = 0;
        obj->unk28 = value;
        obj->unk20 = prev;
    }
    obj->unk04 = -1;
    obj->unk08 = 0;
    obj->unk0C = 0;
    obj->unk10 = 0;
    obj->unk14 = 0;
    obj->unk18 = 0x10000;
    x = arg2->x;
    y = arg2->y;
    z = arg2->z;
    obj->unk2C.x = x;
    obj->unk2C.y = y;
    obj->unk2C.z = z;
    return;
}
