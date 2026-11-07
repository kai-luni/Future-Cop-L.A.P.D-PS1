#include "game/types.h"
typedef struct { s16 x,y,z,pad; } SV;
typedef struct { s32 x,y,z; } V;
typedef struct { char pad[0x24]; V pos; } Transform;
typedef struct { char pad[0x150]; s32 x; char pad2[4]; s32 z; } Data;
typedef struct { char pad[0xac]; Data *data; } Obj;
typedef struct { s16 x,y,z,dx,dy,dz; } Bounds;
extern void func_80010820(void *);
extern void func_8001010C(SV *, SV *, s32);
void func_80051CC4(Obj *obj, V *pos, s32 unused, Bounds *bounds, Transform *trans, void *matrix, s32 flags)
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
    func_80010820(matrix);
    func_8001010C(&local, &rotated, 1);
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
    func_80010820(trans);
    func_8001010C(&step, &output, 1);
    data = obj->data;
    data->x += output.x << 4;
    data->z += output.z << 4;
}
