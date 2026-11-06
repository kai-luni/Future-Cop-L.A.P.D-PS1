#include "game/types.h"

typedef struct { s32 x, y, z; } Vec3;
typedef struct { s16 x, y, z, w; } Vertex;
typedef struct {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[4];
    u8 coord[0x24];
    Vec3 position;
    Vec3 previous;
    u8 pad68[0x38];
    u8 unkA0, unkA1;
    u16 unkA2;
    s16 unkA4;
    u8 padA6[2];
    Vertex vertices[15];
} Object;
typedef struct {
    u8 pad00[0x24];
    u16 width, height;
    s16 x, y, z, offset;
    u8 unk30;
} Data;
typedef struct { u8 pad00[8]; Data *data; } DataHolder;

extern s32 func_8002AD58(s32, Object *, DataHolder *);
extern void func_800188D0(s32, s32, s32, void *);
extern s32 func_80066F78(void *, s32, s32, s32);
extern u32 func_80095900(s32);

s32 func_80041E0C(s32 arg0, Object *obj, DataHolder *holder)
{
    Data *data;
    s32 width, height;
    s32 i;
    register u32 flags __asm__("$2");
    func_8002AD58(2, obj, holder);
    data = holder->data;
    func_800188D0(data->x, data->y, data->z, obj->coord);
    obj->position.y = func_80066F78(&obj->position, 0, 0, 0) + (data->offset << 4);
    obj->previous = obj->position;
    flags = obj->flags;
    obj->unkA4 = -10;
    obj->unkA0 = 0;
    obj->flags = (flags | 0x320005) & ~0x2308;
    obj->unkA1 = data->unk30;
    width = (s16)data->width / 2;
    height = (s16)data->height / 2;
    for (i = 0; i < 15; i++) {
        obj->vertices[i].w = 0;
        obj->vertices[i].z = 0;
    }
    for (i = 0; i < 15; i += 3) {
        obj->vertices[i].x = width;
        obj->vertices[i + 1].x = 0;
        obj->vertices[i + 2].x = -width;
    }
    for (i = 0; i < 3; i++) {
        obj->vertices[i].y = height;
        obj->vertices[i + 3].y = height / 2;
        obj->vertices[i + 6].y = 0;
        obj->vertices[i + 9].y = -height / 2;
        obj->vertices[i + 12].y = -height;
    }
    obj->unkA2 = func_80095900((width * width + height * height) >> 8) >> 2;
    return 0;
}
