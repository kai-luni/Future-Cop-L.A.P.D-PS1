// Categories: Gameplay > Entity Update | Graphics > Model Renderer
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
extern void gGteLoadMatrix(struct Matrix *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");

/**
 * @brief Attaches a model to an entity and fetches its frame vertex.
 *
 * @param obj Object: model at +0x28, matrix at +0x2C, frame at +0x70, output at
 *        +0x5C.
 * @param input Argument block whose source carries the donor model and matrix.
 * @param arg Forwarded to func_8002AD58.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8002AD58(2, obj, input, arg) runs the behaviour, func_80013BD8(&obj->model,
 * src->model, 0) attaches the model, then func_80010750(&obj->matrix) loads the
 * matrix and one func_80010234 call fetches the vertex at
 * `model->frames + (frame * 16) + 8` into obj + 0x5C.
 *
 * @par Likely role
 * The "mount this model on this entity" message: the donor supplies the model, the
 * target keeps its own matrix.
 */
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
    gGteLoadMatrix(&obj->matrix);
    gGteTransformVerticesWide((obj->frame * 16 + 8) + (char *)obj->model->frames, (char *)obj + 0x5c, 1);
}

