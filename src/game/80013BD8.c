// Categories: Graphics > Model Renderer > Instance Setup
#include "game/types.h"

extern int gProjectileInit();

struct Model;

struct Matrix3x3 {
    s32 m[3][3];
};

struct ModelInstance {
    struct Model *model;
    struct Matrix3x3 matrix;
};

extern struct Matrix3x3 gIdentityMatrix __asm__("D_8009AA38");

extern void gInitModelInstance(struct ModelInstance *instance, struct Model *model, u32 options) __asm__("func_80013AF0");

/**
 * @brief Binds a model to an instance and copies the current global rotation
 *        matrix into the instance.
 *
 * @param instance Model instance; +0 the model pointer, +4 the 3x3 matrix.
 * @param model The model to bind (may be 0).
 * @param options Forwarded to the bind helper.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80013AF0(instance, model, options) performs the bind; with a non-null
 * model the nine words of D_8009AA38 are copied into instance->matrix.
 *
 * @par Likely role
 * The spawn-time model setup of the entity handlers: D_8009AA38 holds the
 * last built rotation (the matrix factories write it), so a fresh instance
 * starts with the current orientation.
 *
 * @par Unknown
 * Which code path last wrote D_8009AA38 before a given spawn.
 *
 * @par Callers
 * func_8002A5F4, func_800302E8, func_800317BC, gProjectileInit, func_80036940,
 * func_80037064, func_80038754, func_800516A0, func_800579D4, func_8005920C,
 * func_80083A10.
 */
void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options)
{
    gInitModelInstance(instance, model, options);
    if (model != 0) {
        instance->matrix.m[0][0] = gIdentityMatrix.m[0][0];
        instance->matrix.m[0][1] = gIdentityMatrix.m[0][1];
        instance->matrix.m[0][2] = gIdentityMatrix.m[0][2];
        instance->matrix.m[1][0] = gIdentityMatrix.m[1][0];
        instance->matrix.m[1][1] = gIdentityMatrix.m[1][1];
        instance->matrix.m[1][2] = gIdentityMatrix.m[1][2];
        instance->matrix.m[2][0] = gIdentityMatrix.m[2][0];
        instance->matrix.m[2][1] = gIdentityMatrix.m[2][1];
        instance->matrix.m[2][2] = gIdentityMatrix.m[2][2];
    }
}
