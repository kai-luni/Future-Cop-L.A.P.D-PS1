#include "game/types.h"

struct Model { u8 pad[0x10]; void *data; };
struct Object {
    u8 pad00[0x19];
    u8 flags19;
    u8 pad1A[2];
    s16 state1C;
    u8 pad1E[2];
    u8 column;
    u8 pad21[3];
    u32 flags;
    struct Model *model;
    u8 pad2C[0xC8-0x2C];
    u8 anim[0x14];
    u8 state;
    u8 padDD[0x111-0xDD];
    u8 value;
    u8 anim_index;
};

extern s32 func_8002AD58(s32, struct Object *, void *);
extern s32 func_8002DFC4(struct Object *, void *);
extern s32 func_8002E630(struct Object *, void *);
extern s32 func_8002ACB4(struct Object *, s32);
extern void *func_800448D8(void *, s32, s32, u8);
extern void func_8003B898(u8, struct Object *, s32, u16, s32);

s32 func_8002ED7C(s32 message, struct Object *object, void *data)
{
    switch (message) {
    case 1:
        return 0x114;
    case 2:
        func_8002AD58(message, object, data);
        return func_8002DFC4(object, data);
    case 6:
        return func_8002E630(object, data);
    case 3:
        return func_8002ACB4(object, object->value);
    case 16:
        return object->value;
    case 4:
        if (object->state != 4) {
            if (object->state == 5) return func_8002AD58(message, object, data);
            if (object->model->data == 0 || object->anim_index == 255)
                return func_8002AD58(message, object, data);
            func_800448D8(object->anim, object->anim_index, 1, 1);
            object->state1C = 1;
            object->state = 4;
            object->flags |= 1;
            func_8003B898(object->column, object, 0, (object->flags19 >> 3) & 0x1e, 0);
        }
        return 0;
    default:
        return func_8002AD58(message, object, data);
    }
}
