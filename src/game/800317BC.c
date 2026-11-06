#include "game/types.h"

struct Vec3 { s32 x, y, z; };
struct Matrix { s32 v[9]; };
struct Model { u8 pad[0x34]; void *frames; };
struct Source { u8 pad[0x24]; struct Model *model; struct Matrix *matrix; };
struct Input { u8 pad[8]; struct Source *source; };
struct Object {
    u8 pad[0x28];
    struct Model *model;
    struct Matrix matrix;
    u8 pad50[0x20];
    s16 frame;
    u8 pad72[6];
    struct Vec3 position;
};
extern s32 func_8002AD58(s32, struct Object *, void *, void *);
extern void func_80013BD8(void *, struct Model *, s32);
extern void func_80010750(struct Matrix *);
extern void func_80010234(void *, void *, s32);

void func_800317BC(struct Object *obj, struct Input *input, void *arg)
{
    struct Source *src;
    func_8002AD58(2, obj, input, arg);
    src = input->source;
    func_80013BD8(&obj->model, src->model, 0);
    obj->matrix.v[0] = src->matrix->v[0];
    obj->matrix.v[1] = src->matrix->v[1];
    obj->matrix.v[2] = src->matrix->v[2];
    obj->matrix.v[3] = src->matrix->v[3];
    obj->matrix.v[4] = src->matrix->v[4];
    obj->matrix.v[5] = src->matrix->v[5];
    obj->matrix.v[6] = src->matrix->v[6];
    obj->matrix.v[7] = src->matrix->v[7];
    obj->matrix.v[8] = src->matrix->v[8];
    obj->position = *(struct Vec3 *)&obj->matrix.v[6];
    func_80010750(&obj->matrix);
    func_80010234((obj->frame * 16 + 8) + (char *)obj->model->frames, (char *)obj + 0x5c, 1);
}

