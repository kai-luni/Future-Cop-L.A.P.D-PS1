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

struct Unk40734Light {
    u8 unk00[9];
    u8 red;
    u8 green;
    u8 blue;
};

struct Unk40734Request {
    s32 unk00;
    s32 unk04;
    struct Vec3s32 position;
    u8 kind;
    u8 slot;
    s16 height;
    s16 width;
    s16 depth;
    s16 angle;
    s16 unk1E;
    s16 unk20;
    s8 unk22;
    u8 red;
    u8 green;
    u8 blue;
    u8 unk26[2];
    s16 scale;
    u8 unk2A[6];
};

struct Unk40734Sub {
    u8 unk00[0x34];
    void *model;
};

struct Unk40734Transform {
    struct RotationMatrix matrix;
    struct Vec3s32 pos;
};

struct Unk40734 {
    u8 unk00[0x0C];
    s16 unk0C;
    s8 part;
    u8 unk0F;
    u8 unk10[2];
    s16 unk12;
    u8 unk14[2];
    s16 unk16;
    u8 unk18[2];
    u16 flags1A;
    struct Unk40734Sub *sub;
    u8 unk20[0x14];
    struct RotationMatrix matrix;
    u8 unk58[0x0C];
    s8 unk64;
    u8 pad65;
    u16 unk66;
    u8 unk68[4];
    struct Unk40734Light *light;
    struct RotationPoint point70;
    s16 unk78;
    s16 unk7A;
    s16 unk7C;
    u8 unk7E[9];
    u8 unk87;
    u8 unk88;
    u8 unk89;
};

extern s32 D_8009F318;

extern s32 func_8005A6A4(s32 id, struct Unk40734Request *request,
                         struct Unk40734Light **result);
extern s32 func_8003C7B8(s32 message, void *object, void *arg2, void *arg3);
extern struct Unk40734Transform *func_8003D9F8(void *model, s32 flags, s32 part,
                                               s32 count, void *sub,
                                               void *matrix);
extern void func_8001B148(struct RotationMatrix *matrix,
                          struct RotationPoint *source,
                          struct RotationPoint *destination, s32 count);
extern s32 func_800196C4(void);
extern void func_800498F0(void *object);
extern void func_800181D4(void *block);
extern s32 func_8003C980(s32 message, void *object, void *arg2, void *arg3);

s32 D_80040874(s32 message, struct Unk40734 *object, void *arg2, void *arg3)
{
    struct Unk40734Sub *sub;
    void *model;
    struct Unk40734Transform *transform;

    if (func_8003C7B8(message, object, arg2, arg3) != 0) {
        return 0;
    }

    if (message == 1) {
        return 0;
    }
    if (message == 0) {
        goto case0;
    }
    if (message == 10) {
        goto case10;
    }
    goto deflt;

case0:
    sub = object->sub;
    model = 0;
    if (sub != 0) {
        model = sub->model;
    }
    transform = func_8003D9F8(model, object->flags1A, object->part, 1, sub,
                              &object->matrix);
    if (object->flags1A & 4) {
        func_8001B148(&transform->matrix, &object->point70,
                      &object->point70, 1);
    }
    if (object->unk7A < 0) {
        object->unk7A = ((func_800196C4() & 0xFFF) * -object->unk7A) >> 12;
    }
    if (object->unk7C < 0) {
        object->unk7C = ((func_800196C4() & 0xFFF) * -object->unk7C) >> 12;
    }
    return func_80040734(object, &transform->pos);

case10:
    D_8009F318--;
    if (object->light != 0) {
        func_800498F0(object->light);
        object->light = 0;
    }
    func_800181D4(object);
    return 0;

deflt:
    return func_8003C980(message, object, arg2, arg3);
}
