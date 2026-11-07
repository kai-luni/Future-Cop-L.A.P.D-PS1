#include "game/types.h"

typedef struct { s16 x, y, z; } Vec;
extern void func_800640D8(Vec *, Vec *);
extern s32 func_80063FF8(s16 *, s32);
extern s32 func_8006411C(Vec *, s32, void *);

s32 func_8006429C(void *object, Vec point)
{
    Vec temp, transformed;
    s16 *data = (s16 *)((char *)object + 0x374);
    s32 result;

    temp.x = point.x + 0x1000;
    temp.y = point.y + data[1];
    temp.z = point.z + 0x1000;
    func_800640D8(&temp, &transformed);
    result = func_80063FF8((s16 *)&transformed, data[0] << 4);
    if (result) {
        result |= func_8006411C(&point, 0x1000, data);
        if (result & 2) {
            result |= func_8006411C(&point, 0x800, (char *)data + 0x10) << 4;
            if (!(result & 0x1e0)) result &= ~2;
        }
        point.z += 0x1000;
        if (result & 4) {
            result |= func_8006411C(&point, 0x800, (char *)data + 0x20) << 8;
            if (!(result & 0x1e00)) result &= ~4;
        }
        point.x += 0x1000;
        if (result & 0x10) {
            result |= func_8006411C(&point, 0x800, (char *)data + 0x40) << 16;
            if (!(result & 0x1e0000)) result &= ~0x10;
        }
        point.z -= 0x1000;
        if (result & 8) {
            result |= func_8006411C(&point, 0x800, (char *)data + 0x30) << 12;
            if (!(result & 0x1e000)) result &= ~8;
        }
        if (!(result & 0x1e)) result = 0;
    }
    return result;
}
