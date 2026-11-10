#include "game/types.h"

extern void func_80039E8C(s32 message, void *object, void *data);
extern void func_8003A160(void *object, void *data);
extern s32 func_8002AD58(s32 message, void *object, void *data);

s32 func_8003A844(u32 message, void *object, void *data)
{
    switch (message) {
    case 1:
        return 0x148;
    case 2:
        func_80039E8C(2, object, data);
        return 0;
    case 6:
        func_8003A160(object, data);
        return 0;
    default:
        return func_8002AD58(message, object, data);
    }
}
