#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x78];
    u8 unkA0;
} Object;

extern s32 func_8002AD58(s32 message, Object *object, void *data);
extern s32 func_80041E0C(s32 message, Object *object, void *data);
extern void func_80042128(Object *object);
extern void func_8004228C(Object *object, void *data);

s32 func_80042330(s32 message, Object *object, void *data)
{
    switch (message) {
    case 1:
        return 0x120;
    case 2:
        return func_80041E0C(message, object, data);
    case 3:
        if (!(object->unkA0 & 1)) {
            return 0;
        }
        if (object->flags & 0x20) {
            return 0;
        }
        func_80042128(object);
        return 1;
    case 6:
        if (object->unkA0 & 1) {
            func_8004228C(object, data);
        }
        return 0;
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
        return 0;
    default:
        return func_8002AD58(message, object, data);
    }
}
