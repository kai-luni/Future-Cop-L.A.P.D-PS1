#include "game/types.h"

extern void func_800317BC(void *, void *, void *);
extern s32 func_8002AD58(s32, void *, void *, void *);

s32 func_8003190C(u32 message, void *object, void *data, void *arg)
{
    switch (message) {
    case 1:
        return 0xA0;
    case 2:
        func_800317BC(object, data, arg);
    case 4:
    case 6:
        return 0;
    default:
        return func_8002AD58(message, object, data, arg);
    }
}
