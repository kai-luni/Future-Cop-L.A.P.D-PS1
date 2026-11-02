#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct RotationMatrix {
    s32 values[9];
};

struct RotationPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

struct Unk754B8Callback {
    u8 unk00[4];
    void *(*callback)(s32, void *, void *, s32);
};

struct Unk754B8Sub {
    u8 unk00[0x34];
    void *model;
    u8 unk38[0x10];
    struct Unk754B8Callback *callback;
};

struct PoolNode {
    struct PoolNode *next;
};

struct Pool {
    void *storage;
    struct PoolNode *active;
    struct PoolNode *free;
};

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

struct Unk75478 {
    u8 unk00[0x24];
    void *light;
};

struct Unk754B8 {
    u8 unk00[0x0E];
    s8 part;
    u8 unk0F;
    u8 unk10[2];
    u16 unk12;
    u8 unk14[6];
    u16 flags1A;
    struct Unk754B8Sub *sub;
    u8 unk20[8];
    struct Vec3s32 position;
    struct RotationMatrix matrix;
    u8 unk58[0x0C];
    struct Pool pool;
    u8 unk70[2];
    s8 unk72;
    u8 unk73;
    u8 unk74[2];
    u8 unk76;
    u8 unk77[2];
    u8 unk79;
    u8 unk7A[0x14];
    u16 flags8E;
    u8 unk90[4];
    s16 unk94;
    u8 unk96[4];
    struct RotationPoint point;
};

struct RenderState {
    struct RenderState *next;
    struct Vec3s32 position;
    u16 size;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 red;
    u8 green;
    u8 blue;
    u16 unk18;
    s16 angle;
    u16 unk1C;
    u16 unk1E;
    u16 unk20;
    u8 unk22[2];
    void *light;
    u8 unk28;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    s16 unk2C;
    s16 unk2E;
};

struct Unk754B8Transform {
    struct RotationMatrix matrix;
    struct Vec3s32 pos;
};

extern void func_8001006C(void *source, void *destination, s32 count);
extern void func_80010820(void *matrix);
extern void func_80017668(void *instance, s32 part, s32 *out);
extern void func_800181D4(void *block);
extern s32 func_80019668(void);
extern void func_8001A81C(void *matrix, void *source, void *destination);
extern void func_8001B148(struct RotationMatrix *matrix,
                         struct RotationPoint *source,
                         struct RotationPoint *destination, s32 count);
extern void func_8003BDEC(struct Pool *pool);
extern struct ListNode *func_8003BEF0(struct ListNode *node);
extern s32 func_8003C7B8(s32 message, void *object, void *arg2, void *arg3);
extern s32 func_8003C980(s32 message, void *object, void *arg2, void *arg3);
extern struct Unk754B8Transform *func_8003D9C4(void *object);
extern void func_8003D9F8(void *model, s32 flags, s32 part, s32 count,
                         void *sub, void *matrix);
extern void *jtbl_80011E10[];
extern void func_800752E0(void *object);
extern void func_80075478(struct Unk75478 *object);

void func_800754B8(struct Unk754B8 *object, struct RenderState *render,
                   struct Vec3s32 *position, struct RotationMatrix *matrix)
{
    struct RotationPoint *source;
    struct RotationMatrix *rotation;
    register s32 argument __asm__("$4");
    register s32 extent __asm__("$2");

    render->unk18 = 0;
    render->position = *position;
    render->size = object->unk12;
    render->unk14 = 0;
    render->red = ((u8 *)object)[0x73];
    render->green = ((u8 *)object + render->unk14)[0x76];
    {
        u8 blue = ((u8 *)object + render->unk14)[0x79];
        render->unk12 = 0;
        render->blue = blue;
    }

    if (object->unk72 < 0) {
        u8 enabled;

        render->unk28 = 1;
        __asm__("" : : : "memory");
        extent = 0x4B;
        render->unk2C = extent;
        __asm__("" : : : "memory");
        argument = extent;
        /* Empty allocation/order constraint; emits no instructions. */
        __asm__("" : "=r"(argument)
                   : "0"(argument), "r"(extent)
                   : "memory");
        enabled = render->unk28;
        render->unk29 = 4;
        render->unk2E = argument;
        render->unk2A = enabled;
    }

    if (object->flags8E & 0x8000) {
        source = &object->point;
        if (object->sub != 0) {
            struct Unk754B8Callback *handler;

            handler = object->sub->callback;
            argument = 4;
            if (handler != 0) {
                source = (struct RotationPoint *)handler->callback(
                    argument, object, position, 1);
                if (source == 0) {
                    source = &object->point;
                }
            }
        }

        rotation = &object->matrix;
        if ((object->flags1A & 0x100) == 0) {
            rotation = matrix;
        }
        func_8001B148(rotation, source,
                      (struct RotationPoint *)&render->unk1C, 1);

        if (object->unk94 != 0) {
            render->unk1C +=
                ((func_80019668() >> 20) * object->unk94) >> 12;
            render->unk1E +=
                ((func_80019668() >> 20) * object->unk94) >> 12;
            render->unk20 +=
                ((func_80019668() >> 20) * object->unk94) >> 12;
        }
    }

    if (object->flags8E & 2) {
        func_800752E0(render);
    }
}

/* The second entry is included in the supplied 0x5CC-byte retail range. */
s32 D_80075698(s32 message, struct Unk754B8 *object, void *arg2, void *arg3)
{
    struct Unk754B8Transform *transform;
    struct Unk754B8Transform *current;
    struct Unk754B8Transform local;
    struct Unk754B8Sub *sub;
    void *model;
    s32 first;
    struct RenderState *node;
    struct PoolNode *poolNode;
    struct Vec3s32 vec;
    static void *const label_table[] __attribute__((used)) = {
        &&case0, &&zero, &&deflt, &&deflt, &&deflt, &&deflt,
        &&case6, &&case7, &&case8, &&zero, &&case10
    };

    if (func_8003C7B8(message, object, arg2, arg3) != 0) {
        return 0;
    }

    if ((u32)message >= 11) {
        goto deflt;
    }
    goto *jtbl_80011E10[message];

case0:
        transform = func_8003D9C4(object);

        if ((object->flags1A & 0x100) == 0) {
            sub = object->sub;
            model = 0;
            if (sub != 0) {
                model = sub->model;
            }
            current = transform;

            if (model != 0 && object->part >= 0 && object->part < 4) {
                func_80017668(model, object->part, (s32 *)&local.pos);
                first = object->matrix.values[0];
                local.matrix.values[1] = object->matrix.values[1];
                local.matrix.values[2] = object->matrix.values[2];
                local.matrix.values[3] = object->matrix.values[3];
                local.matrix.values[4] = object->matrix.values[4];
                local.matrix.values[5] = object->matrix.values[5];
                local.matrix.values[6] = object->matrix.values[6];
                local.matrix.values[7] = object->matrix.values[7];
                local.matrix.values[8] = object->matrix.values[8];
                local.matrix.values[0] = first;
                current = &local;
            }

            if (object->flags8E & 0x10) {
                object->position = current->pos;
            } else {
                object->position.x = current->pos.x - transform->pos.x;
                object->position.y = current->pos.y - transform->pos.y;
                object->position.z = current->pos.z - transform->pos.z;
            }
        } else {
            sub = object->sub;
            model = 0;
            if (sub != 0) {
                model = sub->model;
            }
            func_8003D9F8(model, object->flags1A, object->part, 1, sub,
                          &object->matrix);
        }

        if (object->sub != 0 && object->sub->callback != 0) {
            object->sub->callback->callback(3, object, 0, 0);
        }

        node = (struct RenderState *)func_8003BEF0(
            (struct ListNode *)&object->pool);
        if (node != 0) {
            if (object->flags1A & 0x100) {
                func_8001A81C(transform, &object->unk58, &vec);
            } else if (object->flags8E & 0x10) {
                vec = object->position;
            } else {
                func_80010820(transform);
                func_8001006C(&object->position, &vec, 1);
                vec.x = object->position.x + transform->pos.x;
                vec.y = object->position.y + transform->pos.y;
                vec.z = object->position.z + transform->pos.z;
            }

            func_800754B8(object, node, &vec, (struct RotationMatrix *)transform);
        }
        return 1;

case10:
        if (object->sub != 0 && object->sub->callback != 0) {
            object->sub->callback->callback(2, object, 0, 0);
        }

        if (object->flags8E & 2) {
            for (poolNode = object->pool.active; poolNode != 0;
                 poolNode = poolNode->next) {
                func_80075478((struct Unk75478 *)poolNode);
            }
        }

        func_8003BDEC(&object->pool);
        func_800181D4(object);
        return 0;

case7:
        object->flags8E |= 0x2001;
        if (object->flags8E & 0x4000) {
            object->unk0F |= 1;
        }
case6:
        return 1;

case8:
        object->flags8E |= 0x2001;
        goto zero;

deflt:
        return func_8003C980(message, object, arg2, arg3);

case1:
zero:
    return 0;
}
