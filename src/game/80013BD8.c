#include "game/types.h"

struct Model;

struct Matrix3x3 {
    s32 m[3][3];
};

struct ModelInstance {
    struct Model *model;
    struct Matrix3x3 matrix;
};

extern struct Matrix3x3 D_8009AA38;

extern void func_80013AF0(struct ModelInstance *instance, struct Model *model, u32 options);

void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options)
{
    func_80013AF0(instance, model, options);
    if (model != 0) {
        instance->matrix.m[0][0] = D_8009AA38.m[0][0];
        instance->matrix.m[0][1] = D_8009AA38.m[0][1];
        instance->matrix.m[0][2] = D_8009AA38.m[0][2];
        instance->matrix.m[1][0] = D_8009AA38.m[1][0];
        instance->matrix.m[1][1] = D_8009AA38.m[1][1];
        instance->matrix.m[1][2] = D_8009AA38.m[1][2];
        instance->matrix.m[2][0] = D_8009AA38.m[2][0];
        instance->matrix.m[2][1] = D_8009AA38.m[2][1];
        instance->matrix.m[2][2] = D_8009AA38.m[2][2];
    }
}
