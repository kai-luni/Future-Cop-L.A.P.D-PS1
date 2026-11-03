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

u8 func_80040734(struct Unk40734 *object, struct Vec3s32 *position)
{
    struct Unk40734Request request;
    struct Unk40734Light *light;
    s32 length;

    if (object->unk12 < 0x28) {
        request.width = request.depth = 0x50;
    } else {
        request.width = request.depth = object->unk12 << 1;
    }

    request.position = *position;
    request.kind = object->unk66 | 0x40;
    request.height = request.position.y >> 4;
    request.angle = object->unk7A;
    request.unk1E = object->unk7C;
    request.unk22 = -1;
    request.scale = object->unk78;
    request.unk20 = 0;
    request.red = object->unk87;
    request.green = object->unk88;
    request.blue = object->unk89;

    length = func_8005A6A4(object->unk64, &request, &light);
    object->light = light;
    if (light == 0) {
        object->unk0F |= 1;
        return 0;
    }

    if (object->unk16 == 0) {
        object->unk0C = length;
        object->unk16 = length;
    }

    object->light->red = object->unk87;
    object->light->green = object->unk88;
    object->light->blue = object->unk89;
    return 1;
}
