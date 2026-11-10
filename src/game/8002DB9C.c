#include "game/types.h"

struct Model {
    u8 pad00[0x10];
    void *unk10;
};
struct Object {
    u8 pad00[0x28];
    struct Model *model;
    u8 pad2C[0x74];
    u8 unkA0[0xC];
    s16 unkAC;
};
struct Data {
    u8 pad00[0x16];
    u8 unk16;
};

extern s32 func_8002DAA4(struct Object *, void *, s32);
extern s32 func_8002AD58(s32, struct Object *, struct Data *, s32);
extern void func_80044B34(struct Object *, void *, void *, s32);
extern void func_800175F8(void *, s32);

s32 func_8002DB9C(s32 message, struct Object *object, struct Data *data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB4;
    case 2:
        return func_8002DAA4(object, data, arg3);
    case 9:
    case 14:
        return 0;
    case 10:
        return data->unk16 == 1;
    case 6:
        if (object->model->unk10 != 0) {
            func_80044B34(object, object->unkA0, data, 0);
            func_800175F8(&object->model, object->unkAC);
        }
    default:
        return func_8002AD58(message, object, data, arg3);
    }
}
