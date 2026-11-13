#include "game/types.h"

struct Object2F7C8 {
    u8 pad[0xA2];
    u8 flags;
};

extern s32 func_8002EED8(struct Object2F7C8 *, s32, s32);
extern void func_8002F30C(struct Object2F7C8 *, s32, s32);
extern void func_80063194(struct Object2F7C8 *);
extern s32 func_8002AD58(s32, struct Object2F7C8 *, s32, s32);

s32 func_8002F7C8(s32 msg, struct Object2F7C8 *obj, s32 arg2, s32 arg3)
{
    switch ((u32)msg) {
    case 1:
        return 0xC4;
    case 2:
        return func_8002EED8(obj, arg2, arg3);
    case 6:
        func_8002F30C(obj, arg2, arg3);
        return 0;
    case 5:
        if (obj->flags & 4) {
            func_80063194(obj);
        }
    default:
        return func_8002AD58(msg, obj, arg2, arg3);
    }
}
