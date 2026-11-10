#include "game/types.h"

struct Obj41B6C {
    u8 pad[0xC4];
    s16 count;
};

extern s32 func_8002AD58(s32 a0, struct Obj41B6C *a1, s32 a2, s32 a3);
extern void func_80041488(struct Obj41B6C *a0, s32 a1, s32 a2);
extern s32 func_80041880(struct Obj41B6C *a0, s32 a1);
extern void func_80063194(struct Obj41B6C *a0);

s32 func_80041B6C(s32 msg, struct Obj41B6C *obj, s32 a2, s32 a3)
{
    switch (msg) {
    case 1:
        return 0xD0;
    case 2:
        func_80041488(obj, a2, a3);
        return 0;
    case 5:
        func_80063194(obj);
        func_8002AD58(msg, obj, a2, a3);
        return 0;
    case 11:
        if (obj->count < 20) {
            obj->count++;
        }
        func_8002AD58(msg, obj, a2, a3);
        return 0;
    case 6:
        return func_80041880(obj, a2);
    default:
        return func_8002AD58(msg, obj, a2, a3);
    }
}
