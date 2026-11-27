// Categories: Physics > Movement | Gameplay > Navigation
#include "game/types.h"
typedef struct { s16 x,y,z,pad; } SV;
typedef struct { s32 x,y,z; } V;
typedef struct { char pad[0x24]; V pos; } Transform;
typedef struct { char pad[0x150]; s32 x; char pad2[4]; s32 z; } Data;
typedef struct { char pad[0xac]; Data *data; } Obj;
typedef struct { s16 x,y,z,dx,dy,dz; } Bounds;
extern void gGteLoadRotation(void *) __asm__("func_80010820");
void gPushFromPartBox(Obj *obj, V *pos, s32 unused, Bounds *bounds, Transform *trans, void *matrix, s32 flags) __asm__("func_80051CC4");

extern void gGteTransformVertices(SV *, SV *, s32) __asm__("func_8001010C");
/**
 * @brief Moves an object toward a position with a speed, clamped to its bounds.
 *
 * @param obj Object with a data block.
 * @param pos Target position.
 * @param unused Not read.
 * @param bounds Axis-aligned bounds with half-extents dx/dy/dz.
 * @param trans Transform block applied second.
 * @param matrix Matrix for the first transform.
 * @param flags Bit 0x400 forces the speed to 0x4000.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80010820(matrix) + func_8001010C(&local, &rotated, 1) rotate the step, then
 * func_80010820(trans) + func_8001010C(&step, &output, 1) apply the second
 * transform. Each component is negated or zeroed according to the sign test and
 * the `|y| < bounds->dy` comparisons; steps of 0x2400 are the default speed.
 *
 * @par Likely role
 * The "slide along the surface" step of a moving entity: the direction comes from
 * the two transforms and the bounds decide which components are allowed.
 *
 * @par Environment path
 * Collision push against one box-shaped model part, called from the masked
 * part loop of func_80051EB8 for every part bit set in its mask. It rotates
 * the pusher's position relative to the part (func_80010820, func_8001010C
 * with the transposed part matrix), compares it with the box centre and picks
 * a step of +-0x2400 (+-0x4000 with flag 0x400) per axis pointing away from
 * the centre; when the pusher is inside the box vertically the vertical step
 * is dropped and the horizontal axis that is already inside the half extent
 * is zeroed (both halved when inside on x and z). The step is rotated back to
 * world space with the part matrix and added to the x/z position of the
 * object's +0xAC block; no globals are referenced.
 */
void gPushFromPartBox(Obj *obj, V *pos, s32 unused, Bounds *bounds, Transform *trans, void *matrix, s32 flags)
{
    SV local, rotated, step, output;
    Data *data;
    s32 x,y,z, speed, bx,bz;
    register s32 sign __asm__("$3");
    register s32 sum __asm__("$2");
    register s32 ax __asm__("$5");
    register s32 az __asm__("$4");
    local.x = (pos->x - trans->pos.x) >> 4;
    local.y = (pos->y - trans->pos.y) >> 4;
    local.z = (pos->z - trans->pos.z) >> 4;
    gGteLoadRotation(matrix);
    gGteTransformVertices(&local, &rotated, 1);
    speed = 0x2400;
    x = rotated.x - bounds->x;
    y = rotated.y - bounds->y;
    z = rotated.z - bounds->z;
    if (flags & 0x400) speed = 0x4000;
    step.x = speed;
    if (x < 0) step.x = -speed;
    step.y = speed;
    if (y < 0) step.y = -speed;
    step.z = speed;
    if (z < 0) step.z = -speed;
    if ((((y >> 31) + y) ^ (y >> 31)) < bounds->dy) {
        sign = x >> 31;
        sum = sign + x;
        ax = sum ^ sign;
        sign = z >> 31;
        sum = sign + z;
        az = sum ^ sign;
        step.y = 0;
        bx = ax < bounds->dx;
        bz = az < bounds->dz;
        if (bz) {
            if (bx) {
                step.z = step.z / 2;
                step.x = step.x / 2;
                goto done;
            }
        } else if (bx) {
            step.x = 0;
            goto done;
        }
        if (bz) step.z = 0;
    done:;
    }
    gGteLoadRotation(trans);
    gGteTransformVertices(&step, &output, 1);
    data = obj->data;
    data->x += output.x << 4;
    data->z += output.z << 4;
}
