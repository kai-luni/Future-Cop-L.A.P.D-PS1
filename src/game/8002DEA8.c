#include "game/types.h"

struct Child {
    u8 pad00[0x100];
    void *unk100;
};
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
    u8 padAE[6];
    struct Child *unkB4;
};

extern s32 func_8002DC94(struct Object *, void *, s32);
extern s32 func_8002AD58(s32, struct Object *, void *);
extern void func_8002D664(s32, struct Child *, void *, s32);
extern void func_80044B34(struct Object *, void *, void *, s32);
extern void func_800175F8(void *, s32);

s32 func_8002DEA8(s32 message, struct Object *object, void *data, s32 arg3)
{
    s32 flag;
    register s32 cmp __asm__("$2");
    struct Child *child;
    switch (message) {
    case 1:
        return 0xB8;
    case 2:
        return func_8002DC94(object, data, arg3);
    case 10:
        child = object->unkB4;
        cmp = child != data;
        __asm__("" : "=r"(cmp) : "0"(cmp));
        flag = cmp;
        if (child != 0 && child->unk100 == data) {
            flag = 0;
        }
        return flag;
    case 9:
        if (object->unkB4 != 0) {
            return 0;
        }
        return func_8002AD58(message, object, data);
    case 11:
    case 14:
        if (object->unkB4 != 0) {
            func_8002D664(message, object->unkB4, data, arg3);
            return 0;
        }
        return func_8002AD58(message, object, data);
    case 6:
        if (object->model->unk10 != 0) {
            func_80044B34(object, object->unkA0, data, 0);
            func_800175F8(&object->model, object->unkAC);
        }
        return 0;
    default:
        return func_8002AD58(message, object, data);
    }
}
