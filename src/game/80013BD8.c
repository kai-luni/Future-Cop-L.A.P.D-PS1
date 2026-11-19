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

void gBindModelInstanceWithMatrix(struct ModelInstance *instance, struct Model *model, s32 options) __asm__("func_80013BD8");

/**
 * @brief Binds a model to an instance and copies the shared matrix into the
 * instance when a model is supplied.
 *
 * @param instance Instance receiving the model and its matrix.
 * @param model Model to bind, or null.
 * @param options Options forwarded to gInitModelInstance.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gInitModelInstance. If the model argument is non-null, copies nine
 * words from gIdentityMatrix to instance offsets +4 through +0x24.
 *
 * @par Globals
 * Reads gIdentityMatrix when the model is non-null.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_8002A5F4,
 * gInitTerrainEntity, gInitFromSourceModel, gProjectileInit, func_80036940,
 * func_80037064, gInitActorFromSpawn, func_800516A0, func_800579D4,
 * func_8005920C, gSpawnSceneNode.
 *
 * @par Role and path
 * Entity setup callers use this path to initialize both the model binding and
 * the instance matrix.
 *
 * @par Unknown
 * The retail body does not establish which path last wrote the shared matrix.
 */
void gBindModelInstanceWithMatrix(struct ModelInstance *instance, struct Model *model, s32 options)
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
